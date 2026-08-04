#include "activitychartwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include <QMap>
#include <QColor>
#include <QFont>
#include <QLinearGradient>
#include <QFontMetrics>

ActivityChartWidget::ActivityChartWidget(QWidget *parent)
    : QWidget(parent), m_fromDate(QDate::currentDate().addDays(-6)), m_toDate(QDate::currentDate()), m_isHovered(false) {
    setMouseTracking(true);
}

void ActivityChartWidget::setLogs(const QList<AttendanceLog>& logs, const QDate& from, const QDate& to) {
    m_logs = logs;
    m_fromDate = from;
    m_toDate = to;
    update();
}

void ActivityChartWidget::enterEvent(QEnterEvent *event) {
    m_isHovered = true;
    Q_UNUSED(event);
}

void ActivityChartWidget::leaveEvent(QEvent *event) {
    m_isHovered = false;
    m_hoverPos = QPoint(-1, -1);
    update();
    Q_UNUSED(event);
}

void ActivityChartWidget::mouseMoveEvent(QMouseEvent *event) {
    m_hoverPos = event->pos();
    update();
}

void ActivityChartWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawChartOnDevice(painter, rect(), m_logs, m_fromDate, m_toDate, false, m_isHovered ? m_hoverPos : QPoint(-1, -1));
    Q_UNUSED(event);
}

void ActivityChartWidget::drawChartOnDevice(QPainter &painter, const QRect &rect, 
                                          const QList<AttendanceLog> &logs, 
                                          const QDate &from, const QDate &to, 
                                          bool printMode, QPoint mousePos) {
    // 1. Gather all dates in range
    QList<QDate> dates;
    int numDays = from.daysTo(to) + 1;
    if (numDays <= 0) return;
    
    for (QDate d = from; d <= to; d = d.addDays(1)) {
        dates.append(d);
    }

    // 2. Count logs
    QMap<QDate, int> presentCounts;
    QMap<QDate, int> absentCounts;
    for (const QDate &d : dates) {
        presentCounts[d] = 0;
        absentCounts[d] = 0;
    }
    
    for (const AttendanceLog &log : logs) {
        QDate logDate = QDate::fromString(log.getDate(), "dd-MM-yyyy");
        if (logDate.isValid() && presentCounts.contains(logDate)) {
            if (log.getIsPresent() == 1) {
                presentCounts[logDate]++;
            } else {
                absentCounts[logDate]++;
            }
        }
    }

    // 3. Find Max Y-axis value
    int maxVal = 1;
    for (const QDate &d : dates) {
        maxVal = qMax(maxVal, presentCounts[d]);
        maxVal = qMax(maxVal, absentCounts[d]);
    }
    // Round Y-axis to nearest logical multiple of 5
    maxVal = ((maxVal + 4) / 5) * 5;

    // Determine DPI scaling factor to make printable fonts and shapes match screen ratios
    double dpiScale = (double)painter.device()->logicalDpiX() / 96.0;

    // 4. Margins & Dimensions (scale dynamically by DPI)
    int marginL = 60 * dpiScale;
    int marginR = 30 * dpiScale;
    int marginT = 55 * dpiScale;
    int marginB = 40 * dpiScale;

    int chartW = rect.width() - marginL - marginR;
    int chartH = rect.height() - marginT - marginB;
    if (chartW <= 0 || chartH <= 0) return;

    // Background
    if (!printMode) {
        painter.fillRect(rect, QColor(15, 23, 42));
    } else {
        painter.fillRect(rect, Qt::white);
    }

    // Axis Lines & Text Colors
    QColor axisColor = printMode ? QColor(203, 213, 225) : QColor(51, 65, 85);
    QColor textColor = printMode ? QColor(71, 85, 105) : QColor(148, 163, 184);
    QColor gridColor = printMode ? QColor(241, 245, 249) : QColor(30, 41, 59);

    // Font Setup (Set point size relative to device DPI scale)
    QFont labelFont = painter.font();
    labelFont.setPointSize(qMax(8.0, 8.0 * (printMode ? 1.1 : 1.0)));
    painter.setFont(labelFont);

    // 5. Draw Grid lines and Y-axis labels
    int numGridLines = 5;
    for (int i = 0; i <= numGridLines; i++) {
        int val = (maxVal * i) / numGridLines;
        int y = rect.top() + marginT + chartH - (chartH * i) / numGridLines;

        // Draw horizontal grid line
        painter.setPen(QPen(gridColor, qMax(1.0, 1.0 * dpiScale), i == 0 ? Qt::SolidLine : Qt::DashLine));
        painter.drawLine(rect.left() + marginL, y, rect.left() + marginL + chartW, y);

        // Draw Y label
        painter.setPen(textColor);
        QRect labelRect(rect.left(), y - 10 * dpiScale, marginL - 10 * dpiScale, 20 * dpiScale);
        painter.drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, QString::number(val));
    }

    // 6. Compute points for Series lines
    QList<QPointF> presentPoints;
    QList<QPointF> absentPoints;
    double stepX = (numDays > 1) ? (double)chartW / (numDays - 1) : chartW;

    for (int i = 0; i < numDays; i++) {
        QDate d = dates[i];
        double x = rect.left() + marginL + i * stepX;
        
        double yPres = rect.top() + marginT + chartH - (double)(presentCounts[d] * chartH) / maxVal;
        double yAbs = rect.top() + marginT + chartH - (double)(absentCounts[d] * chartH) / maxVal;

        presentPoints.append(QPointF(x, yPres));
        absentPoints.append(QPointF(x, yAbs));
        
        // Draw X-axis label (date text)
        if (numDays < 15 || i % (numDays / 7 + 1) == 0 || i == numDays - 1) {
            painter.setPen(textColor);
            QRect dateRect(x - 40 * dpiScale, rect.top() + marginT + chartH + 10 * dpiScale, 80 * dpiScale, 25 * dpiScale);
            painter.drawText(dateRect, Qt::AlignCenter, d.toString("dd-MM"));
        }
    }

    // 7. Draw Series Gradients and Lines
    QColor presentLineColor(34, 197, 94); // Emerald
    QColor absentLineColor(239, 68, 68);  // Rose

    // Draw Present area & line
    if (numDays > 0) {
        // Draw Fill Path
        QPainterPath presentPath;
        presentPath.moveTo(presentPoints.first().x(), rect.top() + marginT + chartH);
        for (const QPointF &pt : presentPoints) {
            presentPath.lineTo(pt);
        }
        presentPath.lineTo(presentPoints.last().x(), rect.top() + marginT + chartH);
        presentPath.closeSubpath();

        QLinearGradient presGrad(0, rect.top() + marginT, 0, rect.top() + marginT + chartH);
        if (!printMode) {
            presGrad.setColorAt(0, QColor(34, 197, 94, 70));
            presGrad.setColorAt(1, QColor(34, 197, 94, 0));
        } else {
            presGrad.setColorAt(0, QColor(34, 197, 94, 40));
            presGrad.setColorAt(1, QColor(34, 197, 94, 0));
        }
        painter.fillPath(presentPath, presGrad);

        // Draw Line
        painter.setPen(QPen(presentLineColor, qMax(2.0, 2.0 * dpiScale), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QPainterPath linePath;
        linePath.moveTo(presentPoints.first());
        for (int i = 1; i < presentPoints.size(); i++) {
            linePath.lineTo(presentPoints[i]);
        }
        painter.drawPath(linePath);
    }

    // Draw Absent area & line
    if (numDays > 0) {
        // Draw Fill Path
        QPainterPath absentPath;
        absentPath.moveTo(absentPoints.first().x(), rect.top() + marginT + chartH);
        for (const QPointF &pt : absentPoints) {
            absentPath.lineTo(pt);
        }
        absentPath.lineTo(absentPoints.last().x(), rect.top() + marginT + chartH);
        absentPath.closeSubpath();

        QLinearGradient absGrad(0, rect.top() + marginT, 0, rect.top() + marginT + chartH);
        if (!printMode) {
            absGrad.setColorAt(0, QColor(239, 68, 68, 50));
            absGrad.setColorAt(1, QColor(239, 68, 68, 0));
        } else {
            absGrad.setColorAt(0, QColor(239, 68, 68, 30));
            absGrad.setColorAt(1, QColor(239, 68, 68, 0));
        }
        painter.fillPath(absentPath, absGrad);

        // Draw Line
        painter.setPen(QPen(absentLineColor, qMax(2.0, 2.0 * dpiScale), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QPainterPath linePath;
        linePath.moveTo(absentPoints.first());
        for (int i = 1; i < absentPoints.size(); i++) {
            linePath.lineTo(absentPoints[i]);
        }
        painter.drawPath(linePath);
    }

    // 8. Draw Node Points & Handle Interactive Tooltip
    double hoverThreshold = 15.0 * dpiScale;
    int hoveredIndex = -1;

    for (int i = 0; i < numDays; i++) {
        // Present circle
        painter.setPen(QPen(presentLineColor, qMax(1.5, 1.5 * dpiScale)));
        painter.setBrush(printMode ? Qt::white : QColor(15, 23, 42));
        painter.drawEllipse(presentPoints[i], 3.0 * dpiScale, 3.0 * dpiScale);

        // Absent circle
        painter.setPen(QPen(absentLineColor, qMax(1.5, 1.5 * dpiScale)));
        painter.setBrush(printMode ? Qt::white : QColor(15, 23, 42));
        painter.drawEllipse(absentPoints[i], 3.0 * dpiScale, 3.0 * dpiScale);

        // Check if mouse is hovering over this column/x-value
        if (!printMode && mousePos.x() >= 0) {
            if (qAbs(mousePos.x() - presentPoints[i].x()) < hoverThreshold) {
                hoveredIndex = i;
            }
        }
    }

    // 9. Draw hover overlay line and tooltip block
    if (!printMode && hoveredIndex >= 0) {
        QDate hd = dates[hoveredIndex];
        double hX = presentPoints[hoveredIndex].x();
        
        // Draw vertical guide line
        painter.setPen(QPen(QColor(100, 116, 139, 100), 1, Qt::DashLine));
        painter.drawLine(hX, rect.top() + marginT, hX, rect.top() + marginT + chartH);

        // Highlight nodes
        painter.setBrush(presentLineColor);
        painter.drawEllipse(presentPoints[hoveredIndex], 5 * dpiScale, 5 * dpiScale);
        painter.setBrush(absentLineColor);
        painter.drawEllipse(absentPoints[hoveredIndex], 5 * dpiScale, 5 * dpiScale);

        // Draw Tooltip Box
        QString tipText = QString("%1\nPresent: %2\nAbsent: %3")
                            .arg(hd.toString("dd-MM-yyyy"))
                            .arg(presentCounts[hd])
                            .arg(absentCounts[hd]);
        
        QFontMetrics fm = painter.fontMetrics();
        QRect textRect = fm.boundingRect(rect, Qt::AlignLeft, tipText);
        int tipW = textRect.width() + 20 * dpiScale;
        int tipH = textRect.height() + 15 * dpiScale;
        
        int tipX = hX + 10 * dpiScale;
        if (tipX + tipW > rect.right()) {
            tipX = hX - tipW - 10 * dpiScale;
        }
        int tipY = mousePos.y() - tipH / 2;
        tipY = qBound(rect.top() + marginT, tipY, rect.top() + marginT + chartH - tipH);

        QRect tipRect(tipX, tipY, tipW, tipH);
        painter.setPen(QPen(QColor(71, 85, 105), 1));
        painter.setBrush(QColor(30, 41, 59, 240));
        painter.drawRoundedRect(tipRect, 6 * dpiScale, 6 * dpiScale);

        painter.setPen(QColor(241, 245, 249));
        painter.drawText(tipRect.adjusted(10 * dpiScale, 7 * dpiScale, -10 * dpiScale, -7 * dpiScale), Qt::AlignLeft | Qt::AlignTop, tipText);
    }

    // 10. Draw Chart Legends
    painter.setPen(textColor);
    QFont legendFont = painter.font();
    legendFont.setBold(true);
    painter.setFont(legendFont);

    int legY = rect.top() + marginT / 2;
    int legX = rect.left() + marginL + 10 * dpiScale;

    // Present Legend
    painter.setPen(Qt::NoPen);
    painter.setBrush(presentLineColor);
    painter.drawRoundedRect(legX, legY - 5 * dpiScale, 15 * dpiScale, 10 * dpiScale, 2 * dpiScale, 2 * dpiScale);
    painter.setPen(textColor);
    painter.drawText(legX + 22 * dpiScale, legY + 4 * dpiScale, "Present");

    // Absent Legend
    legX += 120 * dpiScale; // safe distance that scales with DPI
    painter.setPen(Qt::NoPen);
    painter.setBrush(absentLineColor);
    painter.drawRoundedRect(legX, legY - 5 * dpiScale, 15 * dpiScale, 10 * dpiScale, 2 * dpiScale, 2 * dpiScale);
    painter.setPen(textColor);
    painter.drawText(legX + 22 * dpiScale, legY + 4 * dpiScale, "Absent");
}
