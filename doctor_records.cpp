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

QList<PrescriptionItem> Doctor::GetRecordPrescriptions(int recordId) {
    QList<PrescriptionItem> items;



    return items;
}

void Doctor::PrintRecord(int recordId) {



}