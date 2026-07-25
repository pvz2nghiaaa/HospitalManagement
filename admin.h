#pragma once
#include <QString>

class Admin
{
public:
    Admin();
    static bool createNewAccount(QString username, QString password, QString fullName,
                                 QString phone, QString role);
};
