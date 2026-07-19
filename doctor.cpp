#include "doctor.h"
#include <QDebug>

Doctor::Doctor() {}

bool Doctor::createMedicalRecord(int patientId, QString diagnosis)
{
    if (!User::GetActiveUser().hasPermission(Permission::createRecord))
    {
        qDebug() << "Access Denied: Current user missing 'createRecord' permission.";
        return false;
    }

    qDebug() << "Medical record created for Patient ID: " << patientId;
    return true;
}

bool Doctor::prescribeMedicine(int patientId, QString medicineName)
{
    if (!User::GetActiveUser().hasPermission(Permission::manageDrugs))
    {
        qDebug() << "Access Denied: Cannot manage/prescribe drugs.";
        return false;
    }

    qDebug() << "Prescribed" << medicineName << "to Patient ID: " << patientId;
    return true;
}
