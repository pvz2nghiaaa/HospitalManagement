#pragma once
#include <QString>
#include "user.h"
#include "permission.h"

enum class RoleTemplate
{
    AdminTemplate,
    DoctorTemplate,
    ReceptionistTemplate
};

class Admin
{
public:
    Admin();
    static bool createNewAccount(QString username, QString password, QString fullName, QString phone, RoleTemplate roleTemp);
};
