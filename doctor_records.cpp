#include "doctor.h"
#include <QSqlQuery>
#include <QVariant>


QList<MedicalRecord> Doctor::SearchRecordsBy(const QString& keyword, const QString& status) {
    QList<MedicalRecord> records;
    QSqlQuery query;
    QString sql = "SELECT m.RecordID, m.Date, m.IsComplete, m.PatientID "
                  "FROM MedicalRecords m "
                  "JOIN Patients p ON m.PatientID = p.ID "
                  "WHERE 1=1 ";

    if (!keyword.isEmpty()) {
        sql += "AND (p.FullName LIKE :keyword OR p.ID = :id_keyword) ";
    }

    if (status != "All") {
        sql += "AND m.IsComplete = :status ";
    }

    sql += "ORDER BY m.RecordID DESC";

    query.prepare(sql);

    if (!keyword.isEmpty()) {
        query.bindValue(":keyword", "%" + keyword + "%");
        query.bindValue(":id_keyword", keyword.toInt());
    }

    if (status == "Completed") {
        query.bindValue(":status", 1);
    } else if (status == "Pending") {
        query.bindValue(":status", 0);
    }
    if (query.exec()) {
        while (query.next()) {
            MedicalRecord rec;
            rec.SetRecordID(query.value("RecordID").toInt())
                .SetDate(query.value("Date").toString())
                .SetIsComplete(query.value("IsComplete").toBool())
                .SetPatientID(query.value("PatientID").toInt());
            records.append(rec);
        }
    } else {
        qDebug() << "SearchRecordsBy error:" << query.lastError().text();
    }

    QSqlQuery check1("SELECT COUNT(*) FROM Patients");
    if (check1.next()) qDebug() << "Tổng số Bệnh nhân trong DB:" << check1.value(0).toInt();

    QSqlQuery check2("SELECT COUNT(*) FROM MedicalRecords");
    if (check2.next()) qDebug() << "Tổng số Bệnh án trong DB:" << check2.value(0).toInt();
    return records;
}


MedicalRecord Doctor::GetRecordDetails(int recordId) {
    MedicalRecord record;
    QSqlQuery query;

    query.prepare("SELECT RecordID, Date, IsComplete, PatientID "
                  "FROM MedicalRecords WHERE RecordID = :id");
    query.bindValue(":id", recordId);

    if (query.exec() && query.next()) {
        record.SetRecordID(query.value("RecordID").toInt())
        .SetDate(query.value("Date").toString())
            .SetIsComplete(query.value("IsComplete").toBool())
            .SetPatientID(query.value("PatientID").toInt());
    } else {
        qDebug() << "GetRecordDetails error:" << query.lastError().text();
    }

    return record;
}


QList<Prescription> Doctor::GetRecordPrescriptions(int recordId) {
    QList<Prescription> items;

    QSqlQuery diagQuery;
    diagQuery.prepare("SELECT DiagnosisID FROM Diagnoses WHERE RecordID = :recId");
    diagQuery.bindValue(":recId", recordId);

    if (diagQuery.exec()) {
        while (diagQuery.next()) {
            int diagId = diagQuery.value("DiagnosisID").toInt();
            QSqlQuery rxQuery;
            rxQuery.prepare("SELECT p.Quantity, p.Note, d.DrugID, d.Name, d.Unit, d.Price "
                            "FROM Prescriptions p "
                            "JOIN Drugs d ON p.DrugID = d.DrugID "
                            "WHERE p.DiagnosisID = :diagId");
            rxQuery.bindValue(":diagId", diagId);

            Prescription rx;
            rx.SetDiagnosisID(diagId);

            QList<Drug> drugs;
            QList<int> quantities;
            QList<QString> notes;

            if (rxQuery.exec()) {
                while (rxQuery.next()) {
                    Drug drug;
                    drug.SetDrugID(rxQuery.value("DrugID").toInt())
                        .SetName(rxQuery.value("Name").toString())
                        .SetUnit(rxQuery.value("Unit").toString())
                        .SetPrice(rxQuery.value("Price").toDouble());

                    drugs.append(drug);
                    quantities.append(rxQuery.value("Quantity").toInt());
                    notes.append(rxQuery.value("Note").toString());
                }
            }

            rx.SetDrugs(drugs).SetQuantities(quantities).SetNotes(notes);

            if (!drugs.isEmpty()) {
                items.append(rx);
            }
        }
    } else {
        qDebug() << "GetRecordPrescriptions error:" << diagQuery.lastError().text();
    }

    return items;
}

void Doctor::GetRecordExtraInfo(int recordId, QString& patientName, QString& doctorName, QString& diagnosis) {
    patientName = "Unknown";
    doctorName = "Unknown";
    diagnosis = "N/A";

    QSqlQuery q;
    q.prepare("SELECT p.FullName AS PatientName, u.FullName AS DoctorName, d.ConditionName "
              "FROM MedicalRecords m "
              "JOIN Patients p ON m.PatientID = p.ID "
              "LEFT JOIN Diagnoses d ON m.RecordID = d.RecordID "
              "LEFT JOIN User u ON d.DoctorID = u.UserID "
              "WHERE m.RecordID = :recId");
    q.bindValue(":recId", recordId);

    if (q.exec() && q.next()) {
        patientName = q.value("PatientName").toString();
        doctorName = q.value("DoctorName").toString();
        diagnosis = q.value("ConditionName").toString();
    } else {
        qDebug() << "GetRecordExtraInfo error:" << q.lastError().text();
    }
}
void Doctor::PrintRecord(int recordId, const QString& filePath) {
    if (filePath.isEmpty()) return;

    MedicalRecord rec = GetRecordDetails(recordId);
    if (rec.GetRecordID() == -1) {
        qDebug() << "Print failed: Record not found!";
        return;
    }

    QString patientName = "Unknown", doctorName = "Unknown", diagnosis = "N/A";

    QSqlQuery q;
    q.prepare("SELECT p.FullName AS PatientName, u.FullName AS DoctorName, d.ConditionName "
              "FROM MedicalRecords m "
              "JOIN Patients p ON m.PatientID = p.ID "
              "LEFT JOIN Diagnoses d ON m.RecordID = d.RecordID "
              "LEFT JOIN User u ON d.DoctorID = u.UserID "
              "WHERE m.RecordID = :recId");
    q.bindValue(":recId", recordId);
    if (q.exec() && q.next()) {
        patientName = q.value("PatientName").toString();
        doctorName = q.value("DoctorName").toString();
        diagnosis = q.value("ConditionName").toString();
    }

    QList<Prescription> rxList = GetRecordPrescriptions(recordId);

    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setResolution(300);

    QPainter painter(&pdfWriter);
    painter.setRenderHint(QPainter::Antialiasing);

    int pageWidth = pdfWriter.width();
    int pageHeight = pdfWriter.height();
    int margin = pageWidth * 0.08;

    int startX = margin;
    int endX = pageWidth - margin;
    int contentWidth = endX - startX;

    int gapSmall = pageHeight * 0.01;
    int gapMed = pageHeight * 0.02;
    int gapLarge = pageHeight * 0.04;

    int col1 = startX;
    int col2 = startX + contentWidth * 0.10;
    int col3 = startX + contentWidth * 0.55;
    int col4 = startX + contentWidth * 0.75;

    QFont titleFont("Arial", 22, QFont::Bold);
    QFont subTitleFont("Arial", 14, QFont::Bold);
    QFont headerFont("Arial", 16, QFont::Bold);
    QFont normalFont("Arial", 12);
    QFont boldFont("Arial", 12, QFont::Bold);
    QFont italicFont("Arial", 12, QFont::StyleItalic);

    int y = margin;

    painter.setFont(titleFont);
    painter.setPen(QColor(6, 182, 212));
    int titleH = painter.fontMetrics().height();
    painter.drawText(QRect(startX, y, contentWidth, titleH), Qt::AlignCenter, "MEDIFLOW HOSPITAL");
    y += titleH + gapSmall;

    painter.setFont(subTitleFont);
    painter.setPen(Qt::darkGray);
    int subH = painter.fontMetrics().height();
    painter.drawText(QRect(startX, y, contentWidth, subH), Qt::AlignCenter, "OFFICIAL MEDICAL RECORD");
    y += subH + gapLarge;

    painter.setPen(Qt::black);
    painter.drawLine(startX, y, endX, y);
    y += gapMed;

    painter.setFont(normalFont);
    int normH = painter.fontMetrics().height();
    int normAscent = painter.fontMetrics().ascent();
    int lineSpacing = normH * 2;

    painter.drawText(startX, y + normAscent, "Patient ID: " + QString::number(rec.GetPatientID()));
    painter.drawText(QRect(startX, y, contentWidth, normH), Qt::AlignRight | Qt::AlignTop, "Patient Name: " + patientName);
    y += lineSpacing;

    painter.drawText(startX, y + normAscent, "Doctor: Dr. " + doctorName);
    painter.drawText(QRect(startX, y, contentWidth, normH), Qt::AlignRight | Qt::AlignTop, "Visit Date: " + rec.GetDate());
    y += lineSpacing;

    painter.setFont(boldFont);
    painter.drawText(startX, y + painter.fontMetrics().ascent(), "Diagnosis: ");
    painter.setFont(normalFont);
    painter.drawText(startX + contentWidth * 0.15, y + painter.fontMetrics().ascent(), diagnosis);
    y += gapLarge;

    painter.drawLine(startX, y, endX, y);
    y += gapMed;

    painter.setFont(headerFont);
    int h16 = painter.fontMetrics().height();
    painter.drawText(startX, y + painter.fontMetrics().ascent(), "PRESCRIPTION");
    y += h16 + gapMed;

    auto drawTableHeader = [&]() {
        painter.setFont(boldFont);
        int boldH = painter.fontMetrics().height();
        int asc = painter.fontMetrics().ascent();
        painter.drawText(col1, y + asc, "No.");
        painter.drawText(col2, y + asc, "Drug Name");
        painter.drawText(col3, y + asc, "Quantity");
        painter.drawText(col4, y + asc, "Instructions");
        y += boldH + gapSmall;
        painter.drawLine(startX, y, endX, y);
        y += gapSmall;
    };

    drawTableHeader();
    painter.setFont(normalFont);
    int index = 1;
    int signatureSpace = gapLarge * 4;

    for (const Prescription& rx : rxList) {
        QList<Drug> drugs = rx.getDrugs();
        QList<int> qtys = rx.getQuantities();
        QList<QString> notes = rx.getNotes();

        for (int i = 0; i < drugs.size(); ++i) {
            int instructionWidth = endX - col4;
            QRect targetRect(col4, y, instructionWidth, 5000);

            // boundingRect sẽ giả lập quá trình in để xem phần hướng dẫn dài bao nhiêu pixel khi bị wrap
            QRect textRect = painter.boundingRect(targetRect, Qt::AlignLeft | Qt::TextWordWrap, notes[i]);

            int rowHeight = qMax(normH * 2, textRect.height() + gapSmall);

            if (y + rowHeight > pageHeight - margin - signatureSpace) {
                pdfWriter.newPage();
                y = margin;
                drawTableHeader();
                painter.setFont(normalFont);
            }

            int textAscent = painter.fontMetrics().ascent();
            painter.drawText(col1, y + textAscent, QString::number(index++));
            painter.drawText(col2, y + textAscent, drugs[i].getName());

            QString qtyText = QString::number(qtys[i]) + " " + drugs[i].getUnit();
            painter.drawText(col3, y + textAscent, qtyText);

            painter.drawText(textRect, Qt::AlignLeft | Qt::TextWordWrap, notes[i]);

            y += rowHeight;
        }
    }

    if (y + signatureSpace > pageHeight - margin) {
        pdfWriter.newPage();
        y = margin;
    } else {
        y += gapLarge;
    }

    int sigBlockWidth = contentWidth * 0.40;
    int sigBlockX = endX - sigBlockWidth;

    painter.setFont(italicFont);
    int itH = painter.fontMetrics().height();
    painter.drawText(QRect(sigBlockX, y, sigBlockWidth, itH), Qt::AlignCenter, "Date: " + rec.GetDate());
    y += itH + gapSmall;

    painter.setFont(boldFont);
    int bH = painter.fontMetrics().height();
    painter.drawText(QRect(sigBlockX, y, sigBlockWidth, bH), Qt::AlignCenter, "Attending Doctor");

    y += gapLarge * 2;

    painter.setFont(normalFont);
    painter.drawText(QRect(sigBlockX, y, sigBlockWidth, normH), Qt::AlignCenter, "Dr. " + doctorName);

    painter.end();
    qDebug() << "Successfully generated PDF at:" << filePath;
}