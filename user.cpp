#include "user.h"

User::User() {}

bool User::initTable(){
    if (!UserLoaded){
        QSqlQuery query;
        UserLoaded = query.exec("CREATE TABLE IF NOT EXISTS User ("
                                      "UserID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                      "Username TEXT NOT NULL, "
                                      "EncryptedPassword TEXT NOT NULL, "
                                      "FullName TEXT NOT NULL, "
                                      "PhoneNumber TEXT, "
                                      "IsActive BOOLEAN DEFAULT TRUE NOT NULL)");
        if (!UserLoaded) {
            qDebug() << "Failed to create User table:" << query.lastError().text();
        } else qDebug() << "User table is initialized";
        return UserLoaded;
    }
    return true;
}
