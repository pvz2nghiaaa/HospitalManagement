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

    int margin = 200;
    int pageWidth = pdfWriter.width();
    int startX = margin;
    int endX = pageWidth - margin;
    int contentWidth = endX - startX;

    painter.setFont(QFont("Arial", 22, QFont::Bold));
    painter.setPen(QColor(6, 182, 212));

    painter.drawText(QRect(startX, margin, contentWidth, 300), Qt::AlignCenter, "MEDIFLOW HOSPITAL");

    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.setPen(Qt::darkGray);
    painter.drawText(QRect(startX, margin + 250, contentWidth, 200), Qt::AlignCenter, "OFFICIAL MEDICAL RECORD");

    int y = margin + 550;
    painter.setPen(Qt::black);
    painter.drawLine(startX, y, endX, y);
    y += 200;

    painter.setFont(QFont("Arial", 12));
    int lineSpacing = 200; // Khoảng cách dòng vừa phải

    painter.drawText(startX, y, "Patient ID: " + QString::number(rec.GetPatientID()));
    painter.drawText(QRect(startX, y, contentWidth, lineSpacing), Qt::AlignRight | Qt::AlignTop, "Patient Name: " + patientName);
    y += lineSpacing;

    painter.drawText(startX, y, "Doctor: Dr. " + doctorName);
    painter.drawText(QRect(startX, y, contentWidth, lineSpacing), Qt::AlignRight | Qt::AlignTop, "Visit Date: " + rec.GetDate());
    y += lineSpacing;

    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(startX, y, "Diagnosis: ");
    painter.setFont(QFont("Arial", 12));
    painter.drawText(startX + 350, y, diagnosis);
    y += lineSpacing + 100;

    painter.drawLine(startX, y, endX, y);
    y += 200;

    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(startX, y, "PRESCRIPTION");
    y += lineSpacing;


    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(startX, y, "No.");
    painter.drawText(startX + 200, y, "Drug Name");
    painter.drawText(startX + 1100, y, "Quantity");
    painter.drawText(startX + 1500, y, "Instructions");

    y += 80;
    painter.drawLine(startX, y, endX, y);
    y += 120;

    painter.setFont(QFont("Arial", 12));
    int index = 1;
    for (const Prescription& rx : rxList) {
        QList<Drug> drugs = rx.getDrugs();
        QList<int> qtys = rx.getQuantities();
        QList<QString> notes = rx.getNotes();

        for (int i = 0; i < drugs.size(); ++i) {
            painter.drawText(startX, y, QString::number(index++));
            painter.drawText(startX + 200, y, drugs[i].getName());

            QString qtyText = QString::number(qtys[i]) + " " + drugs[i].getUnit();
            painter.drawText(startX + 1100, y, qtyText);

            painter.drawText(startX + 1500, y, notes[i]);
            y += lineSpacing;
        }
    }

    painter.end();
    qDebug() << "Successfully generated PDF at:" << filePath;
}