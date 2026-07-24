#pragma once
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "prescription.h"

class Diagnosis
{
private:
    int diagnosisID;
    QString conditionName;
    QString icdCode;
    QString severity;
    int doctorID;
    int recordID;
    Prescription prescription;

public:
    // Constructors
    Diagnosis();
    Diagnosis(QString conditionName, QString icdCode, QString severity, int doctorID, int recordID, Prescription prescription = Prescription());
    Diagnosis(int diagnosisID, QString conditionName, QString icdCode, QString severity, int doctorID, int recordID, Prescription prescription = Prescription());

    // Database initialization
    static bool initTable();

    // Getters
    int getDiagnosisID() const;
    QString getConditionName() const;
    QString getICDCode() const;
    QString getSeverity() const;
    int getDoctorID() const;
    int getRecordID() const;
    Prescription getPrescription() const;
    Prescription& getPrescriptionRef();

    // Setters
    void setDiagnosisID(int id);
    void setConditionName(const QString &cName);
    void setICDCode(const QString &code);
    void setSeverity(const QString &sev);
    void setDoctorID(int docID);
    void setRecordID(int recID);
    void setPrescription(const Prescription &p);

    // Fluent Setters
    Diagnosis& SetDiagnosisID(int id);
    Diagnosis& SetConditionName(const QString &cName);
    Diagnosis& SetICDCode(const QString &code);
    Diagnosis& SetSeverity(const QString &sev);
    Diagnosis& SetDoctorID(int docID);
    Diagnosis& SetRecordID(int recID);
    Diagnosis& SetPrescription(const Prescription &p);
};
