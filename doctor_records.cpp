#include "doctor.h"
#include <QSqlQuery>
#include <QVariant>



QList<MedicalRecord> Doctor::SearchRecordsBy(const QString& keyword, const QString& status, const QString& date) {
    QList<MedicalRecord> records;



    return records;
}

MedicalRecord Doctor::GetRecordDetails(int recordId) {
    MedicalRecord record;



    return record;
}

QList<Prescription> Doctor::GetRecordPrescriptions(int recordId) {
    QList<Prescription> items;



    return items;
}

void Doctor::PrintRecord(int recordId) {



}