#include "doctor.h"
#include <QDebug>
#include <QDate>

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
    query.prepare("SELECT User.UserID, User.FullName FROM User "
                  "INNER JOIN AttendanceLogs ON User.UserID = AttendanceLogs.EmployeeID "
                  "WHERE (User.Role = 'Doctor' OR User.Role = 'doctor') "
                  "AND User.IsActive = 1 "
                  "AND AttendanceLogs.Date = :today "
                  "AND AttendanceLogs.IsPresent = 1");
    query.bindValue(":today", QDate::currentDate().toString("dd-MM-yyyy"));

    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
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
