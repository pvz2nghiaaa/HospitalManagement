#include "doctor.h"
#include <QDebug>

Doctor::Doctor(int nID, QString username, QString password, QString fullName, QString phone, bool isActive)
    : User(nID, username, password, fullName, phone, isActive, "Doctor")
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

QList<std::pair<int, QString>> Doctor::getAvailableDoctors()
{
    QList<std::pair<int, QString>> doctors;
    QSqlQuery query;
    query.prepare("SELECT UserID, FullName FROM User WHERE (Role = 'Doctor' OR Role = 'doctor') AND IsActive = 1");
    if (query.exec()) {
        while (query.next()) {
            int id = query.value("UserID").toInt();
            QString name = query.value("FullName").toString();
            doctors.append({id, name});
        }
    } else {
        qDebug() << "Failed to fetch available doctors:" << query.lastError().text();
    }
    return doctors;
}

bool Doctor::isDoctorAvailable(int doctorId)
{
    QList<std::pair<int, QString>> doctors = getAvailableDoctors();
    for (const auto &doc : doctors) {
        if (doc.first == doctorId) return true;
    }
    return false;
}
