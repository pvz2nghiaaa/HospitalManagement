#include "medicalrecord.h"
#include "user.h"
#include "permission.h"

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
    if (!User::GetActiveUser().hasPermission(Permission::viewLog)){
        qDebug() << "User does not have permission to view dashboard info";
        return 0;
    }
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
MedicalRecord& MedicalRecord::SetPatientName(const QString& name) { patientName = name; return *this; }
MedicalRecord& MedicalRecord::SetPatientID(int id) { this->PatientID = id; return *this; }
// MedicalRecord& MedicalRecord::SetDoctorName(const QString& name) {  doctorName = name; return *this; }

MedicalRecord& MedicalRecord::SetDiagnosis(const QList<QString>& diag) { diagnosis = diag; return *this; }
MedicalRecord& MedicalRecord::AddDiagnosis(const QString& singleDiag) { diagnosis.append(singleDiag); return *this; }

// Getters
int MedicalRecord::GetRecordID() const { return RecordID; }
QString MedicalRecord::GetDate() { return Date; }
bool MedicalRecord::GetIsComplete() { return IsComplete; }
int MedicalRecord::GetPatientID() { return PatientID; }
QString MedicalRecord::GetPatientName() { return patientName; }
// QString MedicalRecord::GetDoctorName() { return doctorName; }

QList<QString> MedicalRecord::GetDiagnosis() { return diagnosis; }

int MedicalRecord::createRecord(int patientID, QString date)
{
    QSqlQuery query;
    query.prepare("INSERT INTO MedicalRecords (Date, IsComplete, PatientID) VALUES (:date, 0, :patientID)");
    query.bindValue(":date", date);
    query.bindValue(":patientID", patientID);
    if (query.exec()) {
        int recId = query.lastInsertId().toInt();
        qDebug() << "MedicalRecord created successfully with RecordID:" << recId;
        return recId;
    }
    qDebug() << "Failed to create MedicalRecord:" << query.lastError().text();
    return -1;
}