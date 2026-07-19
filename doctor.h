#pragma once
#include "user.h"
#include "permission.h"

class Doctor
{
public:
    Doctor();
    static bool createMedicalRecord(int patientId, QString diagnosis);
    static bool prescribeMedicine(int patientId, QString medicineName);
};
