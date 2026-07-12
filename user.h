#ifndef USER_H
#define USER_H
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
public:
    User();
    static bool initTable();
};

#endif // USER_H
