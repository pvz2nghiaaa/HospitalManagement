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
    int ID;
    QString Username;
    QString EncryptedPassword;
    QList<Permission> ListPermission;
    QString FullName;
    QString PhoneNumber;
    bool IsActive;
    QString Role;
protected:
    User(int nID,
         QString Username,
         QString EncryptedPassword,
         QString FullName,
         QString PhoneNumber,
         bool IsActive,
         QString Role);
    User();
public:
    static bool initTable();
    bool hasPermission(Permission);
    static bool login(QString nUsername, QString nPassword);
    static void logout();
    static User* getCurrentUser();
    static QString GetEncryptPassword(QString nPassword);
    static int GetTotalStaff();
    void appendPermission(Permission);
    void erasePermission(Permission);
    // fast set attribute
    User& SetID(int nID);
    User& SetUsername(QString nUsername);
    User& SetEncryptedPassword(QString nEncryptedPassword);
    User& SetFullName(QString nFullName);
    User& SetPhoneNumber(QString nPhoneNumber);
    User& SetIsActive(bool nIsActive);
    User& SetRole(QString role);
    bool UpdatePermissionFromDatabase();
    // get methods
    int GetID();
    QString GetUsername();
    QString GetEncryptedPassword();
    QString GetFullName();
    QString GetPhoneNumber();
    bool GetIsActive();
    QString GetRole();
    // prevent copy and assignment constructors
    User(const User&) = delete;
    User& operator=(const User&) = delete;
    User(User&&) = delete;
    User& operator=(const User&&) = delete;
    // Meyers' Singleton
    static User& GetActiveUser();
};
