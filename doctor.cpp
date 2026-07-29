#include "doctor.h"
#include <QDebug>

Doctor::Doctor(int nID, QString username, QString password, QString fullName, QString phone, bool isActive)
    : User(nID, username, password, fullName, phone, isActive)
{

    this->appendPermission(Permission(Permission::createRecord));
    this->appendPermission(Permission(Permission::viewRecord));
    this->appendPermission(Permission(Permission::editRecord));
    this->appendPermission(Permission(Permission::manageDrugs));
    qDebug() << "Doctor" << username << "initialized with 4 default permissions.";
}

bool Doctor::prescribeMedicine(int patientId, QString medicineName)
{
    if (!this->hasPermission(Permission(Permission::manageDrugs)))
    {
        qDebug() << "Access Denied: Doctor lacks manageDrugs permission.";
        return false;
    }
    qDebug() << "Prescribed" << medicineName << "for Patient ID:" << patientId;
    return true;
}
