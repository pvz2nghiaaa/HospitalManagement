#include "medicalrecord.h"

MedicalRecord::MedicalRecord() : RecordID(-1), IsComplete(false), PatientID(-1) {}

MedicalRecord::MedicalRecord(int nRecordID, QString nDate, bool nIsComplete, int nPatientID) :
    RecordID(nRecordID), Date(nDate), IsComplete(nIsComplete), PatientID(nPatientID) {}

bool MedicalRecord::initTable() {
    static bool TableLoaded = false;
    if (!TableLoaded) {
        QSqlQuery query;
        TableLoaded = query.exec("CREATE TABLE IF NOT EXISTS MedicalRecords ("
                                 "RecordID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "Date TEXT, "
                                 "IsComplete BOOLEAN, "
                                 "PatientID INTEGER, "
                                 "FOREIGN KEY(PatientID) REFERENCES Patients(ID))");
        if (!TableLoaded) {
            qDebug() << "Failed to create MedicalRecords table:" << query.lastError().text();
        } else {
            qDebug() << "MedicalRecords table is initialized";
        }
        return TableLoaded;
    }
    return true;
}

bool MedicalRecord::MarkComplete() {
    if (IsComplete == true) return false;

    IsComplete = true;

    if (RecordID == -1) return false;

    QSqlQuery query;
    query.prepare("UPDATE MedicalRecords SET IsComplete = 1 WHERE RecordID = :id");
    query.bindValue(":id", RecordID);

    if (!query.exec()) {
        qDebug() << "Failed to mark record complete:" << query.lastError().text();
        return false;
    }
    return true;
}

int MedicalRecord::GetTotalRecord(){
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) AS records FROM MedicalRecords");
    if (query.exec() && query.next()){
        return query.value("records").toInt();
    }
    qDebug() << "Failed to get total records from MedicalRecords";
    return 0;
}

// Setters
MedicalRecord& MedicalRecord::SetRecordID(int nRecordID) { RecordID = nRecordID; return *this; }
MedicalRecord& MedicalRecord::SetDate(QString nDate) { Date = nDate; return *this; }
MedicalRecord& MedicalRecord::SetIsComplete(bool nIsComplete) { IsComplete = nIsComplete; return *this; }
MedicalRecord& MedicalRecord::SetPatientID(int nPatientID) { PatientID = nPatientID; return *this; }

// Getters
int MedicalRecord::GetRecordID() { return RecordID; }
QString MedicalRecord::GetDate() { return Date; }
bool MedicalRecord::GetIsComplete() { return IsComplete; }
int MedicalRecord::GetPatientID() { return PatientID; }