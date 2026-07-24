#pragma once
#include <QList>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "medicalrecord.h"

class RecordManager
{
public:
    RecordManager();

    // Core operations
    bool createRecords(MedicalRecord record);

    // Updated search methods
    QList<MedicalRecord> searchRecordsByDate(QString date);
    QList<MedicalRecord> searchRecordsByPatientID(int patientID);
};