#include "permission.h"

bool Permission::PermissionLoaded = false;

Permission::Permission() {

}
constexpr Permission::Permission(Type type) : Val(type){}
constexpr Permission::operator Type() const {return Val;}

bool Permission::initTable(){
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
bool Permission::changeUserPermission(User &manager, User &other, Permission per, bool isOn){
    if (!manager.hasPermission(Permission::changePermission))
        return false;
    if (isOn){
        if (other.hasPermission(per))
            return true;
        manager.ListPermission.append(per);
        QSqlQuery query;
        query.prepare("INSERT INTO Permission(UserID, PermissionType) VALUES(:id, :type)");
        query.bindValue(":id", other.id);
        query.bindValue(":type", per);
        return query.exec();
    }
    if (!other.hasPermission(per))
        return true;
    manager.ListPermission.removeAll(per);
    QSqlQuery query;
    query.prepare("DELETE FROM Permission WHERE UserID = :id and PermissionType = :type");
    query.bindValue(":id", other.id);
    query.bindValue(":type", per);
    return query.exec();
}