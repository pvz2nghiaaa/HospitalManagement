#pragma once
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class MedicalRecord
{
private:
    int RecordID;
    QString Date;
    bool IsComplete;
    int PatientID;

<<<<<<< HEAD
    QString doctorName;
    QString diagnosis;

=======
>>>>>>> Doctor_clinical
public:
    MedicalRecord();
    MedicalRecord(int nRecordID, QString nDate, bool nIsComplete, int nPatientID);

    static bool initTable();
    bool MarkComplete();
    static int GetTotalRecord();

    // Fast set attributes (Meyers' Singleton/Fluent style from User convention)
    MedicalRecord& SetRecordID(int nRecordID);
    MedicalRecord& SetDate(QString nDate);
    MedicalRecord& SetIsComplete(bool nIsComplete);
    MedicalRecord& SetPatientID(int nPatientID);

<<<<<<< HEAD
    MedicalRecord& SetDoctorName(const QString& name);
    MedicalRecord& SetDiagnosis(const QString& diag);
=======
>>>>>>> Doctor_clinical
    // Get methods
    int GetRecordID();
    QString GetDate();
    bool GetIsComplete();
    int GetPatientID();
<<<<<<< HEAD
    QString GetDoctorName();
    QString GetDiagnosis();

=======
>>>>>>> Doctor_clinical
};