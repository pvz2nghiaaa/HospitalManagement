#include "user.h"

User::User() {}


User::User(int nID, QString nUsername, QString nEncryptedPassword,
           QString nFullName, QString nPhoneNumber, bool nIsActive):
    id(nID), Username(nUsername), EncryptedPassword(nEncryptedPassword),
    Fullname(nFullname), PhoneNumber(nPhoneNumber), IsActive(nIsActive){}

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

bool User::hasPermission(Permission per){
    for (int i = 0; i < ListPermission.size(); i++)
        if (per == ListPermission[i])
            return true;
    return false;
}
void User::login(Qstring nUsername, Qstring nPassword) {
    if (activeUser)
        delete activeUser;

    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE Username = :username and EncryptedPassword = :encrypted");
    query.bindValue(":username", nUsername);
    query.bindValue(":encrypted", GetEncryptPassword(nPassword));
    if (query.exec() && query.next()){
        activeUser = new User(query.value("UserID").toInt(),
                              query.value("Username").toString(),
                              query.value("EncryptedPassword").toString(),
                              query.value("FullName").toString(),
                              query.value("PhoneNumber").toString(),
                              query.value("IsActive").toBool());
    } else {
        activeUser = nullptr;
    }
}
void User::logout();
User* User::getCurrentUser();

QString User::GetEncryptPassword(QString nPassword){
    return nPassword; // Warnign***: no hash, will change later
}

void User::~User(){
    if (activeUser)
        delete activeUser;
}
