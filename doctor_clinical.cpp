#include "doctor.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QVariant>

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
    query.prepare("INSERT INTO Diseases (ConditionName, ICDCode) VALUES (?, ?)");
    query.addBindValue(conditionName);
    query.addBindValue(icdCode);

    if (query.exec()) {
        qDebug() << "Added new disease successfully:" << conditionName;
        return true;
    } else {
        qDebug() << "Error AddNewDisease:" << query.lastError().text();
        return false;
    }
}

bool Doctor::SaveDiagnosis(int diagnosisID, const QString& conditionName, const QString& icdCode, const QString& severity, const QString& doctorID, const QString& recordID) {

    QSqlQuery query;
    query.prepare("INSERT INTO Diagnoses (DiagnosisID, ConditionName, ICDCode, Severity, DoctorID, RecordID) "
                  "VALUES (?, ?, ?, ?, ?, ?)");

    query.addBindValue(diagnosisID);
    query.addBindValue(conditionName);
    query.addBindValue(icdCode);
    query.addBindValue(severity);
    query.addBindValue(doctorID);
    query.addBindValue(recordID);

    if (query.exec()) {
        qDebug() << "Saved diagnosis successfully for RecordID:" << recordID;
        return true;
    }
    else
    {
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

    if (!QSqlDatabase::database().isOpen())
    {
        qWarning() << "[Doctor::GetDrugsList] ERROR: Database not connected!";
        return drugs;
    }
    QSqlQuery query;
    QString sql = "SELECT drug_name FROM drugs ORDER BY drug_name ASC;";
    if (!query.exec(sql))
    {
        qWarning() << "[Doctor::GetDrugsList] DB QUERY ERROR:" << query.lastError().text();
        return drugs;
    }
    while (query.next())
    {
        drugs.append(query.value("drug_name").toString());
    }

    return drugs;
}

bool Doctor::AddPrescriptionItem(int recordId, int drugId, int quantity, const QString& instruction) {


    return false;
}

bool Doctor::RemovePrescriptionItem(int prescriptionItemId) {



    return false;
}

bool Doctor::SavePrescription(int recordId, const QString& dateIssued, const QList<Prescription>& items) {


    return false;
}