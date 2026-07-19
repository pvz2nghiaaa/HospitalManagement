#pragma once
#include "user.h"
#include "permission.h"

class Admin
{
public:
    Admin();
    static bool createNewAccount(QString username, QString password);
};
