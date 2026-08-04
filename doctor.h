#pragma once
#include "user.h"
#include "permission.h"

class Doctor : public User
{
public:
    Doctor(int nID, QString username, QString password, QString fullName, QString phone, bool isActive);
    bool prescribeMedicine(int patientId, QString medicineName);

    static QList<std::pair<int, QString>> getAvailableDoctors();
    static QList<std::pair<int, QString>> getAvailableDoctorsByIDOrName(QString searchItem);
    static bool isDoctorAvailable(int doctorId);
};
