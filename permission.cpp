#include "permission.h"
#include "user.h"
using namespace std;

Permission::Permission() {

}

bool Permission::initTable(){
    static bool PermissionLoaded = false;
    if (!PermissionLoaded){
        QSqlQuery query;
        PermissionLoaded = query.exec("CREATE TABLE IF NOT EXISTS Permission ("
                                  "PermissionID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                  "UserID INTEGER, "
                                  "PermissionType INTEGER, "
                                  "FOREIGN KEY(UserID) REFERENCES User(UserID))");
        if (!PermissionLoaded) {
            qDebug() << "Failed to create Permission table:" << query.lastError().text();
        } else qDebug() << "Permission table is initialized";
        return PermissionLoaded;
    }
    return true;
}
bool Permission::changeUserPermission(User &other, Permission per, bool isOn){
    if (!User::GetActiveUser().hasPermission(Permission::changePermission))
        return false;
    if (isOn){
        if (other.hasPermission(per))
            return true;
        other.appendPermission(per);
        QSqlQuery query;
        query.prepare("INSERT INTO Permission(UserID, PermissionType) VALUES(:id, :type)");
        query.bindValue(":id", other.GetID());
        query.bindValue(":type", per.toUnderlying());
        return query.exec();
    }
    if (!other.hasPermission(per))
        return true;
    other.erasePermission(per);
    QSqlQuery query;
    query.prepare("DELETE FROM Permission WHERE UserID = :id and PermissionType = :type");
    query.bindValue(":id", other.GetID());
    query.bindValue(":type", per.toUnderlying());
    return query.exec();
}

Permission::Permission(Type type) : Val(type){}

Permission::operator Type() const {
    return Val;
}

int Permission::toUnderlying() const {
    return qToUnderlying(Val);
}

QList<Permission> Permission::GetActiveUserPermission(){
    QList<Permission> list;
    QSqlQuery query;
    query.prepare("SELECT PermissionType FROM Permission WHERE UserID = :id");
    query.bindValue(":id", User::GetActiveUser().GetID());
    if (query.exec()){
        while(query.next())
            list.append(static_cast<Permission::Type>(query.value("PermissionType").toInt()));
    }
    else qDebug() << query.lastError().text();
    return list;
}

QList<Permission> Permission::GetActiveUserPermission(int userID){
    QList<Permission> list;
    if (!User::GetActiveUser().hasPermission(Permission::manageUsers))
        return list;

    QSqlQuery query;
    query.prepare("SELECT PermissionType FROM Permission WHERE UserID = :id");
    query.bindValue(":id", userID);
    if (query.exec()){
        while(query.next())
            list.append(static_cast<Permission::Type>(query.value("PermissionType").toInt()));
    }
    else qDebug() << query.lastError().text();
    return list;
}

QString Permission::permissionToReadableString(Permission::Type type) {
    switch (type) {
    case Permission::viewLog:         return "View Logs";
    case Permission::addLog:          return "Add Logs";
    case Permission::changePermission:return "Change Permissions";
    case Permission::manageUsers:     return "Manage Users";
    case Permission::createRecord:    return "Create Records";
    case Permission::viewRecord:      return "View Records";
    case Permission::editRecord:      return "Edit Records";
    case Permission::createPatient:   return "Register Patients";
    case Permission::editPatient:     return "Edit Patients";
    case Permission::createInvoice:   return "Create Invoices";
    case Permission::viewInvoice:     return "View Invoices";
    case Permission::manageDrugs:     return "Manage Drugs";
    default:                          return "Unknown";
    }
}