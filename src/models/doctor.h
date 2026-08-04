#pragma once
#include <QPdfWriter>
#include <QPageSize>
#include <QPainter>
#include <QFont>

#include "user.h"
#include "permission.h"
#include "patient.h"
#include "prescription.h"
#include "medicalrecord.h"
#include "attendancelog.h"


class Doctor : public User{
public:
    static int GetDrugIdByName(const QString& drugName);

    Doctor(int nID, QString username, QString password, QString fullName, QString phone, bool isActive);
    bool prescribeMedicine(int patientId, QString medicineName);

    static QList<Patient> SearchPatientBy(const QString& keyword);
    static Patient GetPatientDetails(int patientID);
    static bool UpdatePatientInfo(int id, const QString& fullName, const QString& birthDate, const QString& sex, const QString& address);
    static bool GetLatestDiagnosis(int patientId, QString& conditionName, QString& icdCode, QString& severity);

    static QList<QString> GetDiseasesList();
    static bool AddNewDisease(const QString& conditionName, const QString& icdCode);
    static QString GetICDCodeByName(const QString& conditionName);
    static bool SaveDiagnosis(int recordId, const QString& conditionName, const QString& icdCode, const QString& severity, const QString& clinicalNote, const QString& dateDiagnosed);
    static bool MarkRecordComplete(int recordId);
    static bool CheckDoctorPatientPermission(int patientId);
    static QList<QString> GetDrugsList();
    static bool AddPrescriptionItem(int recordId, int drugId, int quantity, const QString& instruction);
    static bool RemovePrescriptionItem(int Prescription);
    static bool SavePrescription(int recordId, const QString& dateIssued, const QList<Prescription>& items);

    static QList<MedicalRecord> SearchRecordsBy(const QString& keyword, const QString& status, const QString& date);
    static MedicalRecord GetRecordDetails(int recordId);
    static QList<Prescription> GetRecordPrescriptions(int recordId);
    static void GetRecordExtraInfo(int recordId, QString& patientName, QList<QString>& doctorNames, QList<QString>& doctorIds, QList<QString>& diagnoses);
    static void PrintRecord(int recordId, const QString& filePath);
    static void GetAllDiagnosesForRecord(const MedicalRecord& rec, QList<QString>& outDoctors, QList<QString>& outDiagnoses);

    static User& GetMyProfileInfo();
    static QList<Permission> GetMyPermissions();
    static QList<AttendanceLog> SearchMyActivityLogs(const QString& dateFrom, const QString& dateTo);

    static QList<std::pair<int, QString>> getAvailableDoctors();
    static QList<std::pair<int, QString>> getAvailableDoctorsByIDOrName(QString searchItem);
    static bool isDoctorAvailable(int doctorId);
};
