#include "admin.h"
#include <QDebug>
#include <QVariant>
#include "permission.h"
#include "user.h"

Admin::Admin() {}

bool Admin::createNewAccount(QString username, QString password, QString fullName, QString phone, RoleTemplate roleTemp)
{
    if (!User::GetActiveUser().hasPermission(Permission::manageUsers))
    {
        qDebug() << "[Admin] Access Denied: Current user does not have permission to create an account.";
        return false;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO User (Username, EncryptedPassword, FullName, PhoneNumber, IsActive) "
                  "VALUES (:user, :pass, :name, :phone, 1)");
    query.bindValue(":user", username);
    query.bindValue(":pass", User::GetEncryptPassword(password));
    query.bindValue(":name", fullName);
    query.bindValue(":phone", phone);

    if (query.exec())
    {
        int newUserId = query.lastInsertId().toInt();
        qDebug() << "[Admin] Successfully created UserID:" << newUserId;
        QList<Permission::Type> defaultPerms;
        switch (roleTemp)
        {
        case RoleTemplate::DoctorTemplate:
            defaultPerms << Permission::createRecord << Permission::viewRecord
                         << Permission::editRecord << Permission::manageDrugs;
            break;
        case RoleTemplate::ReceptionistTemplate:
            defaultPerms << Permission::createPatient << Permission::editPatient
                         << Permission::viewRecord;
            break;
        case RoleTemplate::AdminTemplate:
            defaultPerms << Permission::manageUsers << Permission::changePermission
                         << Permission::viewLog << Permission::addLog;
            break;
        }
        QSqlQuery permQuery;
        permQuery.prepare("INSERT INTO Permission (UserID, PermissionType) VALUES (:uid, :ptype)");

        for (Permission::Type pType : defaultPerms)
        {
            permQuery.bindValue(":uid", newUserId);
            permQuery.bindValue(":ptype", static_cast<int>(pType));
            if (!permQuery.exec())
                qDebug() << "[Admin] Error assigning permission" << pType << ":" << permQuery.lastError().text();
        }
        qDebug() << "[Admin] Successfully assigned" << defaultPerms.size() << "default permissions.";
        return true;
    }
    else
    {
        qDebug() << "[Admin] Error creating account:" << query.lastError().text();
        return false;
    }
}
