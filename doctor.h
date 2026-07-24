#pragma once
#include "user.h"
#include "permission.h"

class Doctor : public User{
public:
    Doctor(int nID, QString username, QString password, QString fullName, QString phone, bool isActive);
    bool prescribeMedicine(int patientId, QString medicineName);

    static QList<Patient> SearchPatientBy(const QString& keyword);
    static Patient GetPatientDetails(int patientID);
    static bool UpdatePatientInfo(int id, const QString& fullName, const QString& birthDate, const QString& sex, const QString& address);

    static QList<QString> GetDiseasesList();
    static bool AddNewDisease(const QString& conditionName, const QString& icdCode);
    static bool SaveDiagnosis(int recordId, const QString& conditionName, const QString& icdCode, const QString& severity, const QString& clinicalNote, const QString& dateDiagnosed);
    static bool MarkRecordComplete(int recordId);

    static QList<QString> GetDrugsList();
    static bool AddPrescriptionItem(int recordId, int drugId, int quantity, const QString& instruction);
    static bool RemovePrescriptionItem(int prescriptionItemId);
    static bool SavePrescription(int recordId, const QString& dateIssued, const QList<PrescriptionItem>& items);

    static QList<MedicalRecord> SearchRecordsBy(const QString& keyword, const QString& status, const QString& date);
    static MedicalRecord GetRecordDetails(int recordId);
    static QList<PrescriptionItem> GetRecordPrescriptions(int recordId);
    static void PrintRecord(int recordId);

    static Profile GetMyProfileInfo();
    static QList<Permission> GetMyPermissions();
    static QList<ActivityLog> SearchMyActivityLogs(const QString& dateFrom, const QString& dateTo);
};
