#pragma once
#include "user.h"
#include "permission.h"

class Doctor : public User
{
public:
    Doctor(int nID, QString username, QString password, QString fullName, QString phone, bool isActive);
    bool prescribeMedicine(int patientId, QString medicineName);
};
