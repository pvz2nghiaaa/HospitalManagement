#pragma once
#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QList>
#include "permission.h"
class User
{
private:
    int id;
    QString Username;
    QString EncryptedPassword;
    QList<Permission> ListPermission;
    QString FullName;
    QString PhoneNumber;
    bool IsActive;
    static bool UserLoaded;
    static User* activeUser;
    User(int nID,
         QString Username,
         QString EncryptedPassword,
         QString FullName,
         QString PhoneNumber,
         bool IsActive);
public:
    static bool initTable();
    bool hasPermission(Permission);
    static void login(QString nUsername, QString nPassword);
    static void logout();
    static User* getCurrentUser();
    static QString GetEncryptPassword(QString nPassword);
    ~User();
};
