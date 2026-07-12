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