#include "doctor.h"
#include <QSqlQuery>
#include <QVariant>



QList<MedicalRecord> Doctor::SearchRecordsBy(const QString& keyword, const QString& status, const QString& date) {
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

    if (!date.isEmpty()) {
        sql += "AND m.Date = :date ";
    }

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
    if (!date.isEmpty()) {
        query.bindValue(":date", date);
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
    diagQuery.prepare("SELECT DiagnosisID FROM Diagnosis WHERE RecordID = :recId");
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
              "LEFT JOIN Diagnosis d ON m.RecordID = d.RecordID "
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
    painter.setPen(Qt::black);

    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(0, 500, "MEDIFLOW HOSPITAL - MEDICAL RECORD");
    painter.drawLine(0, 700, 4000, 700);

    painter.setFont(QFont("Arial", 12));
    int y = 1200;
    int lineSpacing = 300;

    painter.drawText(0, y, "Patient ID: " + QString::number(rec.GetPatientID()));
    painter.drawText(2000, y, "Patient Name: " + patientName);
    y += lineSpacing;

    painter.drawText(0, y, "Doctor: " + doctorName);
    painter.drawText(2000, y, "Date: " + rec.GetDate());
    y += lineSpacing;

    painter.drawText(0, y, "Diagnosis: " + diagnosis);
    y += lineSpacing * 2;

    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(0, y, "PRESCRIPTION:");
    y += lineSpacing;

    painter.setFont(QFont("Arial", 12));
    int index = 1;
    for (const Prescription& rx : rxList) {
        QList<Drug> drugs = rx.getDrugs();
        QList<int> qtys = rx.getQuantities();
        QList<QString> notes = rx.getNotes();

        for (int i = 0; i < drugs.size(); ++i) {
            painter.drawText(200, y, QString::number(index++) + ". " + drugs[i].getName());

            QString qtyText = QString::number(qtys[i]) + " " + drugs[i].getUnit();
            painter.drawText(3000, y, "Qty: " + qtyText);
            y += lineSpacing;

            if (!notes[i].isEmpty()) {
                painter.drawText(400, y, "Note: " + notes[i]);
                y += lineSpacing;
            }
        }
    }

    painter.end();
    qDebug() << "Successfully generated PDF at:" << filePath;
}