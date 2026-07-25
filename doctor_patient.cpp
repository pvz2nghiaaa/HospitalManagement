#include "doctor.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>


QList<Patient> Doctor::SearchPatientBy(const QString& keyword) {
    QList<Patient> results;



    return results;
}

Patient Doctor::GetPatientDetails(int patientID) {
    Patient p;



    return p;
}

bool Doctor::UpdatePatientInfo(int id, const QString& fullName, const QString& birthDate, const QString& sex, const QString& address) {



    return false;
}