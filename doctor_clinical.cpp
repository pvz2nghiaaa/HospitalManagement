#include "doctor.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QVariant>

QList<QString> Doctor::GetDiseasesList() {
    QList<QString> diseases;



    return diseases;
}

bool Doctor::AddNewDisease(const QString& conditionName, const QString& icdCode) {



    return false;
}

bool Doctor::SaveDiagnosis(int recordId, const QString& conditionName, const QString& icdCode, const QString& severity, const QString& clinicalNote, const QString& dateDiagnosed) {



    return false;
}

bool Doctor::MarkRecordComplete(int recordId) {


    return false;
}


QList<QString> Doctor::GetDrugsList() {
    QList<QString> drugs;



    return drugs;
}

bool Doctor::AddPrescriptionItem(int recordId, int drugId, int quantity, const QString& instruction) {


    return false;
}

bool Doctor::RemovePrescriptionItem(int prescriptionItemId) {



    return false;
}

bool Doctor::SavePrescription(int recordId, const QString& dateIssued, const QList<PrescriptionItem>& items) {


    return false;
}