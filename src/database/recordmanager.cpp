#include "recordmanager.h"

RecordManager::RecordManager() {}

bool RecordManager::createRecords(MedicalRecord record) {
    QSqlQuery query;
    query.prepare("INSERT INTO MedicalRecords (Date, IsComplete, PatientID) "
                  "VALUES (:date, :isComplete, :patientID)");
    query.bindValue(":date", record.GetDate());
    query.bindValue(":isComplete", record.GetIsComplete());
    query.bindValue(":patientID", record.GetPatientID());

    if (!query.exec()) {
        qDebug() << "Failed to create record:" << query.lastError().text();
        return false;
    }
    return true;
}

// Searches medical records by a specific date string
QList<MedicalRecord> RecordManager::searchRecordsByDate(QString date) {
    QList<MedicalRecord> results;
    QSqlQuery query;

    query.prepare("SELECT * FROM MedicalRecords WHERE Date = :date");
    query.bindValue(":date", date);

    if (query.exec()) {
        while (query.next()) {
            MedicalRecord record(
                query.value("RecordID").toInt(),
                query.value("Date").toString(),
                query.value("IsComplete").toBool(),
                query.value("PatientID").toInt()
                );
            results.append(record);
        }
    } else {
        qDebug() << "Failed to search records by date:" << query.lastError().text();
    }
    return results;
}

// Searches medical records belonging to a specific Patient ID
QList<MedicalRecord> RecordManager::searchRecordsByPatientID(int patientID) {
    QList<MedicalRecord> results;
    QSqlQuery query;

    query.prepare("SELECT * FROM MedicalRecords WHERE PatientID = :patientID");
    query.bindValue(":patientID", patientID);

    if (query.exec()) {
        while (query.next()) {
            MedicalRecord record(
                query.value("RecordID").toInt(),
                query.value("Date").toString(),
                query.value("IsComplete").toBool(),
                query.value("PatientID").toInt()
                );
            results.append(record);
        }
    } else {
        qDebug() << "Failed to search records by patient ID:" << query.lastError().text();
    }
    return results;
}