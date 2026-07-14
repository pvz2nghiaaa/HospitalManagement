#include "user.h"

User::User() {}


User::User(int nID, QString nUsername, QString nEncryptedPassword,
           QString nFullName, QString nPhoneNumber, bool nIsActive):
    ID(nID), Username(nUsername), EncryptedPassword(nEncryptedPassword),
    FullName(nFullName), PhoneNumber(nPhoneNumber), IsActive(nIsActive){}

bool User::initTable(){
    static bool UserLoaded = false;
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

bool User::hasPermission(Permission per){
    for (int i = 0; i < ListPermission.size(); i++)
        if (per.toUnderlying() == ListPermission[i].toUnderlying())
            return true;
    return false;
}

bool User::login(QString nUsername, QString nPassword) {

    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE Username = :username and EncryptedPassword = :encrypted");
    query.bindValue(":username", nUsername);
    query.bindValue(":encrypted", GetEncryptPassword(nPassword));
    if (query.exec() && query.next()){
        GetActiveUser().SetID(query.value("UserID").toInt())
            .SetUsername(query.value("Username").toString())
            .SetEncryptedPassword(query.value("EncryptedPassword").toString())
            .SetFullName(query.value("FullName").toString())
            .SetPhoneNumber(query.value("PhoneNumber").toString())
            .SetIsActive(query.value("IsActive").toBool());
        return true;
    }
    return false;
}

void User::logout(){

}

QString User::GetEncryptPassword(QString nPassword){
    return nPassword; // Warning***: no hash, will change later
}

void User::appendPermission(Permission per){
    if (!GetActiveUser().hasPermission(Permission::manageUsers))
        return ;
    ListPermission.append(per);
}

void User::erasePermission(Permission per){
    if (!GetActiveUser().hasPermission(Permission::manageUsers))
        return ;
    ListPermission.removeAll(per);
}

int User::GetID(){
    return ID;
}

User& User::GetActiveUser(){
    static User instance;
    return instance;
}

User& User::SetID(int nID){
    ID = nID;
    return *this;
}
User& User::SetUsername(QString nUsername){
    Username = nUsername;
    return *this;
}
User& User::SetEncryptedPassword(QString nEncryptedPassword){
    EncryptedPassword = nEncryptedPassword;
    return *this;
}
User& User::SetFullName(QString nFullName){
    FullName = nFullName;
    return *this;
}
User& User::SetPhoneNumber(QString nPhoneNumber){
    PhoneNumber = nPhoneNumber;
    return *this;
}
User& User::SetIsActive(bool nIsActive){
    IsActive = nIsActive;
    return *this;
}