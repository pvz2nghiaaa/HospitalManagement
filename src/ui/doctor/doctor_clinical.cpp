#include "doctor.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QVariant>

int Doctor::GetDrugIdByName(const QString& drugName) {
    if (!QSqlDatabase::database().isOpen()) return -1;

    QSqlQuery query;
    query.prepare("SELECT DrugID FROM Drugs WHERE Name = :name");
    query.bindValue(":name", drugName);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

QList<QString> Doctor::GetDiseasesList() {
    QList<QString> diseases;
    QSqlQuery query("SELECT ConditionName FROM Diseases");

    while (query.next()) {
        diseases.append(query.value(0).toString());
    }
    return diseases;
}

bool Doctor::AddNewDisease(const QString& conditionName, const QString& icdCode) {
    QSqlQuery query;
    query.prepare("INSERT OR IGNORE INTO Diseases (ConditionName, ICDCode) VALUES (:name, :code)");
    query.bindValue(":name", conditionName);
    query.bindValue(":code", icdCode);
    return query.exec();
}

bool Doctor::GetLatestDiagnosis(int patientId, QString& conditionName, QString& icdCode, QString& severity) {
    if (!QSqlDatabase::database().isOpen()) return false;

    QSqlQuery query;
    query.prepare("SELECT d.ConditionName, d.ICDCode, d.Severity "
                  "FROM Diagnoses d "
                  "JOIN MedicalRecords m ON d.RecordID = m.RecordID "
                  "WHERE m.PatientID = :patientId "
                  "ORDER BY m.RecordID DESC LIMIT 1");

    query.bindValue(":patientId", patientId);
    if (query.exec() && query.next()) {
        conditionName = query.value("ConditionName").toString();
        icdCode = query.value("ICDCode").toString();
        severity = query.value("Severity").toString();
        return true;
    }

    return false;
}
QString Doctor::GetICDCodeByName(const QString& conditionName) {
    QSqlQuery query;
    query.prepare("SELECT ICDCode FROM Diseases WHERE ConditionName = :name");
    query.bindValue(":name", conditionName);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return "";
}

bool Doctor::SaveDiagnosis(int recordId, const QString& conditionName, const QString& icdCode, const QString& severity, const QString& clinicalNote, const QString& dateDiagnosed) {
    if (!QSqlDatabase::database().isOpen()) return false;

    int doctorId = User::GetActiveUser().GetID();

    QSqlQuery query;
    query.prepare("INSERT INTO Diagnoses (ConditionName, ICDCode, Severity, DoctorID, RecordID, ClinicalNote) "
                  "VALUES (:name, :code, :severity, :doctorId, :recordId, :note)");

    query.bindValue(":name", conditionName);
    query.bindValue(":code", icdCode);
    query.bindValue(":severity", severity);
    query.bindValue(":doctorId", doctorId);
    query.bindValue(":recordId", recordId);
    query.bindValue(":note", clinicalNote);

    if (query.exec()) {
        return true;
    } else {
        qDebug() << "=== LỖI DATABASE KHI LƯU CHẨN ĐOÁN ===";
        qDebug() << query.lastError().text();
        qDebug() << "Câu query đang chạy:" << query.lastQuery();
        return false;
    }
}
bool Doctor::MarkRecordComplete(int patientId) {
    if (!QSqlDatabase::database().isOpen()) return false;
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM MedicalRecords WHERE PatientID = ?");
    checkQuery.addBindValue(patientId);
    int count = 0;
    if (checkQuery.exec() && checkQuery.next())
    {
        count = checkQuery.value(0).toInt();
    }
    QSqlQuery query;
    if (count > 0)
    {
        query.prepare("UPDATE MedicalRecords SET IsComplete = 1 WHERE PatientID = ?");
    }
    else
    {
        query.prepare("INSERT INTO MedicalRecords (PatientID, IsComplete) VALUES (?, 1)");
    }
    query.addBindValue(patientId);

    if (!query.exec())
    {
        qWarning() << "[DB UPDATE ERROR]:" << query.lastError().text();
        return false;
    }

    qDebug() << "Successfully completed record for PatientID:" << patientId;
    return true;
}
QList<QString> Doctor::GetDrugsList() {
    QList<QString> drugs;

    // qDebug() << "ok";
    if (!QSqlDatabase::database().isOpen())
    {
        qWarning() << "[Doctor::GetDrugsList] ERROR: Database not connected!";
        return drugs;
    }
    QSqlQuery query;
    QString sql = "SELECT Name FROM Drugs ORDER BY Name ASC;";
    if (!query.exec(sql))
    {
        qWarning() << "[Doctor::GetDrugsList] DB QUERY ERROR:" << query.lastError().text();
        return drugs;
    }
    while (query.next())
    {
        drugs.append(query.value("Name").toString());
        // qDebug() << "ok2";
    }

    return drugs;
}

// bool Doctor::AddPrescriptionItem(int recordId, int drugId, int quantity, const QString& instruction) {
//     QSqlQuery query;

//     query.prepare("INSERT INTO Prescriptions (DiagnosisID, DrugID, Quantity, Note) "
//                   "VALUES (:diagnosisId, :drugId, :quantity, :note)");

//     query.bindValue(":diagnosisId", recordId);
//     query.bindValue(":drugId", drugId);
//     query.bindValue(":quantity", quantity);
//     query.bindValue(":note", instruction);

//     if (!query.exec()) {
//         qDebug() << "Failed AddPrescriptionItem:" << query.lastError().text();
//         return false;
//     }
//     return true;
// }

// bool Doctor::RemovePrescriptionItem(int prescriptionItemId) {
//     QSqlQuery query;
//     query.prepare("DELETE FROM Prescriptions WHERE DetailID = :id");
//     query.bindValue(":id", prescriptionItemId);

//     if (!query.exec()) {
//         qDebug() << "Failed RemovePrescriptionItem:" << query.lastError().text();
//         return false;
//     }
//     return true;
// }
bool Doctor::SavePrescription(int recordId, const QString& dateIssued, const QList<Prescription>& items) {
    QSqlQuery clearQuery;
    clearQuery.prepare("DELETE FROM Prescriptions WHERE DiagnosisID = :diagId");
    clearQuery.bindValue(":diagId", recordId);
    clearQuery.exec();

    QSqlDatabase::database().transaction();

    for (const Prescription& p : items) {
        int currentDiagID = (p.getDiagnosisID() != -1) ? p.getDiagnosisID() : recordId;

        QList<Drug> drugs = p.getDrugs();
        QList<int> quantities = p.getQuantities();
        QList<QString> notes = p.getNotes();

        for (int i = 0; i < drugs.size(); ++i) {
            QSqlQuery query;
            query.prepare("INSERT INTO Prescriptions (DiagnosisID, DrugID, Quantity, Note) "
                          "VALUES (:diagnosisId, :drugId, :quantity, :note)");

            query.bindValue(":diagnosisId", currentDiagID);
            query.bindValue(":drugId", drugs[i].getDrugID());
            query.bindValue(":quantity", quantities[i]);
            query.bindValue(":note", notes[i]);

            if (!query.exec()) {
                qDebug() << "Failed SavePrescription:" << query.lastError().text();
                QSqlDatabase::database().rollback();
                return false;
            }
        }
    }

    QSqlDatabase::database().commit();
    return true;
}