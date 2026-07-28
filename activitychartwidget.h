#pragma once

#include <QWidget>
#include <QDate>
#include <QList>
#include <QPoint>
#include "attendancelog.h"

class ActivityChartWidget : public QWidget {
    Q_OBJECT
private:
    QList<AttendanceLog> m_logs;
    QDate m_fromDate;
    QDate m_toDate;
    QPoint m_hoverPos;
    bool m_isHovered;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

public:
    explicit ActivityChartWidget(QWidget *parent = nullptr);
    
    void setLogs(const QList<AttendanceLog>& logs, const QDate& from, const QDate& to);

    // Static rendering helper to share with the PDF generator
    static void drawChartOnDevice(QPainter &painter, const QRect &rect, 
                                  const QList<AttendanceLog> &logs, 
                                  const QDate &from, const QDate &to, 
                                  bool printMode, QPoint mousePos = QPoint(-1, -1));
};
