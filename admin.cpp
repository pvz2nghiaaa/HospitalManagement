#include "admin.h"
#include <QDebug>

Admin::Admin() {}

bool Admin::createNewAccount(QString username, QString password)
{
    if (!User::GetActiveUser().hasPermission(Permission::manageUsers))
    {
        qDebug() << "Access Denied: Current user missing 'manageUsers' permission.";
        return false;
    }
    qDebug() << "Account created successfully: " << username;
    return true;
}
