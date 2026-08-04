#pragma once
#include <QString>

class Admin
{
public:
    Admin();
    static bool createNewAccount(QString username, QString password, QString fullName,
                                 QString phone, QString role);
    static bool updateAccount(int id, QString fullName, QString phone, QString role, bool isActive, QString newPassword = "");
    static bool deleteAccount(int id);
};
