#include "diagnosis.h"

Diagnosis::Diagnosis() : diagnosisID(-1), doctorID(-1), recordID(-1) {}

Diagnosis::Diagnosis(QString conditionName, QString icdCode, QString severity, int doctorID, int recordID, Prescription prescription)
    : diagnosisID(-1), conditionName(conditionName), icdCode(icdCode), severity(severity),
      doctorID(doctorID), recordID(recordID), prescription(prescription) {}

Diagnosis::Diagnosis(int diagnosisID, QString conditionName, QString icdCode, QString severity, int doctorID, int recordID, Prescription prescription)
    : diagnosisID(diagnosisID), conditionName(conditionName), icdCode(icdCode), severity(severity),
      doctorID(doctorID), recordID(recordID), prescription(prescription) {}

bool Diagnosis::initTable() {
    static bool tableLoaded = false;
    if (!tableLoaded) {
        QSqlQuery query;
        tableLoaded = query.exec("CREATE TABLE IF NOT EXISTS Diagnoses ("
                                 "DiagnosisID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "ConditionName TEXT, "
                                 "ICDCode TEXT, "
                                 "Severity TEXT, "
                                 "DoctorID INTEGER, "
                                 "RecordID INTEGER, "
                                 "FOREIGN KEY(DoctorID) REFERENCES User(UserID), "
                                 "FOREIGN KEY(RecordID) REFERENCES MedicalRecords(RecordID))");
        if (!tableLoaded) {
            qDebug() << "Failed to create Diagnoses table:" << query.lastError().text();
        } else {
            qDebug() << "Diagnoses table is initialized";
        }
        return tableLoaded;
    }
    return true;
}

// Getters
int Diagnosis::getDiagnosisID() const { return diagnosisID; }
QString Diagnosis::getConditionName() const { return conditionName; }
QString Diagnosis::getICDCode() const { return icdCode; }
QString Diagnosis::getSeverity() const { return severity; }
int Diagnosis::getDoctorID() const { return doctorID; }
int Diagnosis::getRecordID() const { return recordID; }
Prescription Diagnosis::getPrescription() const { return prescription; }
Prescription& Diagnosis::getPrescriptionRef() { return prescription; }

// Setters
void Diagnosis::setDiagnosisID(int id) { diagnosisID = id; }
void Diagnosis::setConditionName(const QString &cName) { conditionName = cName; }
void Diagnosis::setICDCode(const QString &code) { icdCode = code; }
void Diagnosis::setSeverity(const QString &sev) { severity = sev; }
void Diagnosis::setDoctorID(int docID) { doctorID = docID; }
void Diagnosis::setRecordID(int recID) { recordID = recID; }
void Diagnosis::setPrescription(const Prescription &p) { prescription = p; }

// Fluent Setters
Diagnosis& Diagnosis::SetDiagnosisID(int id) { diagnosisID = id; return *this; }
Diagnosis& Diagnosis::SetConditionName(const QString &cName) { conditionName = cName; return *this; }
Diagnosis& Diagnosis::SetICDCode(const QString &code) { icdCode = code; return *this; }
Diagnosis& Diagnosis::SetSeverity(const QString &sev) { severity = sev; return *this; }
Diagnosis& Diagnosis::SetDoctorID(int docID) { doctorID = docID; return *this; }
Diagnosis& Diagnosis::SetRecordID(int recID) { recordID = recID; return *this; }
Diagnosis& Diagnosis::SetPrescription(const Prescription &p) { prescription = p; return *this; }
