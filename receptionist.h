#pragma once
#include "user.h"
#include "permission.h"

class Receptionist
{
public:
    Receptionist();
    static bool registerNewPatient(QString patientName);
};
