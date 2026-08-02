#include "user.h"
#include <QDebug>
#include "permission.h"

User::User() {}


User::User(int nID, QString nUsername, QString nEncryptedPassword,
           QString nFullName, QString nPhoneNumber, bool nIsActive, QString role):
    ID(nID), Username(nUsername), EncryptedPassword(nEncryptedPassword),
    FullName(nFullName), PhoneNumber(nPhoneNumber), IsActive(nIsActive), Role(role){}

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
                                      "Role TEXT, "
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
            .SetIsActive(query.value("IsActive").toBool())
            .SetRole(query.value("Role").toString());
        if (GetActiveUser().GetIsActive() && GetActiveUser().UpdatePermissionFromDatabase())
            return true;
        logout();
        return false;
    }
    return false;
}

void User::logout(){
    GetActiveUser().SetID(-1)
        .SetUsername("")
        .SetEncryptedPassword("")
        .SetFullName("")
        .SetPhoneNumber("")
        .SetIsActive(false);
}

int User::GetTotalStaff(){
    if (!User::GetActiveUser().hasPermission(Permission::viewLog)){
        qDebug() << "User does not have permission to view staff info";
        return 0;
    }
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) AS valid_staffs FROM User "
                  "WHERE Role NOT IN ('Admin') AND IsActive = 1");
    if (query.exec() && query.next()){
        return query.value("valid_staffs").toInt();
    }
    qDebug() << "Failed to get total staff from User";
    return 0;
}

vector<tuple<int, QString, QString, QString, bool, QString>> User::GetAllUser() {
    vector<tuple<int, QString, QString, QString, bool, QString>> listUser;

    if (!User::GetActiveUser().hasPermission(Permission::manageUsers) && User::GetActiveUser().GetRole() != "Receptionist")
        return listUser;

    QSqlQuery query;
    query.prepare("SELECT * FROM User");
    if (query.exec())
        while(query.next()){
            listUser.push_back(
                tuple<int, QString, QString, QString, bool, QString> {
                    query.value("UserID").toInt(),
                    query.value("Username").toString(),
                    query.value("FullName").toString(),
                    query.value("PhoneNumber").toString(),
                    query.value("IsActive").toBool(),
                    query.value("Role").toString()
                }
            );
        }
    else qDebug() << "Error: " << query.lastError().text();
    return listUser;
}

vector<tuple<int, QString, QString, QString, bool, QString>> User::SearchUserBy(QString name) {
    vector<tuple<int, QString, QString, QString, bool, QString>> listUser;

    if (!User::GetActiveUser().hasPermission(Permission::manageUsers))
        return listUser;

    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE Fullname = :name");
    query.bindValue(":name", name);
    if (query.exec())
        while(query.next()){
            listUser.push_back(
                tuple<int, QString, QString, QString, bool, QString> {
                    query.value("UserID").toInt(),
                    query.value("Username").toString(),
                    query.value("FullName").toString(),
                    query.value("PhoneNumber").toString(),
                    query.value("IsActive").toBool(),
                    query.value("Role").toString()
                }
                );
        }
    else qDebug() << "Error: " << query.lastError().text();
    return listUser;
}

vector<tuple<int, QString, QString, QString, bool, QString>> User::SearchUserBy(QString name, QString role) {
    vector<tuple<int, QString, QString, QString, bool, QString>> listUser;

    if (!User::GetActiveUser().hasPermission(Permission::manageUsers))
        return listUser;

    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE Fullname = :name AND Role = :role");
    query.bindValue(":name", name);
    query.bindValue(":role", role);
    if (query.exec())
        while(query.next()){
            listUser.push_back(
                tuple<int, QString, QString, QString, bool, QString> {
                    query.value("UserID").toInt(),
                    query.value("Username").toString(),
                    query.value("FullName").toString(),
                    query.value("PhoneNumber").toString(),
                    query.value("IsActive").toBool(),
                    query.value("Role").toString()
                }
                );
        }
    else qDebug() << "Error: " << query.lastError().text();
    return listUser;
}
vector<tuple<int, QString, QString, QString, bool, QString>> User::SearchUserByRole(QString role) {
    vector<tuple<int, QString, QString, QString, bool, QString>> listUser;

    if (!User::GetActiveUser().hasPermission(Permission::manageUsers))
        return listUser;

    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE Role = :role");
    query.bindValue(":role", role);
    if (query.exec())
        while(query.next()){
            listUser.push_back(
                tuple<int, QString, QString, QString, bool, QString> {
                    query.value("UserID").toInt(),
                    query.value("Username").toString(),
                    query.value("FullName").toString(),
                    query.value("PhoneNumber").toString(),
                    query.value("IsActive").toBool(),
                    query.value("Role").toString()
                }
                );
        }
    else qDebug() << "Error: " << query.lastError().text();
    return listUser;
}

vector<tuple<int, QString, QString, QList<Permission> > > User::SearchPermissionBy(QString name) {
    vector<tuple<int, QString, QString, QList<Permission> > > listPermission;

    if (!User::GetActiveUser().hasPermission(Permission::manageUsers))
        return listPermission;

    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE Fullname = :name");
    query.bindValue(":name", name);
    if (query.exec())
        while(query.next()){
            listPermission.push_back(
                tuple<int, QString, QString, QList<Permission> > {
                    query.value("UserID").toInt(),
                    query.value("FullName").toString(),
                    query.value("Role").toString(),
                    Permission::GetUserPermission(query.value("UserID").toInt())
                }
            );
        }
    else qDebug() << "Error: " << query.lastError().text();
    return listPermission;
}

vector<tuple<int, QString, QString, QList<Permission> > > User::SearchPermissionBy(QString name, QString role) {
    vector<tuple<int, QString, QString, QList<Permission> > > listPermission;

    if (!User::GetActiveUser().hasPermission(Permission::manageUsers))
        return listPermission;

    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE Fullname = :name AND Role = :role");
    query.bindValue(":name", name);
    query.bindValue(":role", role);
    if (query.exec())
        while(query.next()){
            listPermission.push_back(
                tuple<int, QString, QString, QList<Permission> > {
                    query.value("UserID").toInt(),
                    query.value("FullName").toString(),
                    query.value("Role").toString(),
                    Permission::GetUserPermission(query.value("UserID").toInt())
                }
                );
        }
    else qDebug() << "Error: " << query.lastError().text();
    return listPermission;
}

vector<tuple<int, QString, QString, QList<Permission> > > User::SearchPermissionByRole(QString role) {
    vector<tuple<int, QString, QString, QList<Permission> > > listPermission;

    if (!User::GetActiveUser().hasPermission(Permission::manageUsers))
        return listPermission;

    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE Role = :role");
    query.bindValue(":role", role);
    if (query.exec())
        while(query.next()){
            listPermission.push_back(
                tuple<int, QString, QString, QList<Permission> > {
                    query.value("UserID").toInt(),
                    query.value("FullName").toString(),
                    query.value("Role").toString(),
                    Permission::GetUserPermission(query.value("UserID").toInt())
                }
            );
        }
    else qDebug() << "Error: " << query.lastError().text();
    return listPermission;
}

bool User::UpdatePermissionFromDatabase(){
    ListPermission = Permission::GetActiveUserPermission();
    if (ListPermission.empty())
        return false;
    return true;
}

vector<tuple<int, QString, QString, QList<Permission> > > User::GetAllUserPermission() {
    vector<tuple<int, QString, QString, QList<Permission> > > list;
    if (!User::GetActiveUser().hasPermission(Permission::manageUsers))
        return list;

    QSqlQuery query;
    query.prepare("SELECT * FROM User");
    if (query.exec())
        while(query.next()){
            list.push_back(
                tuple<int, QString, QString, QList<Permission> > {
                    query.value("UserID").toInt(),
                    query.value("FullName").toString(),
                    query.value("Role").toString(),
                    Permission::GetUserPermission(query.value("UserID").toInt())
                }
                );
        }
    else qDebug() << "Error: " << query.lastError().text();
    return list;
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
User& User::SetRole(QString role){
    Role = role;
    return *this;
}
int User::GetID(){
    return ID;
}
QString User::GetUsername(){
    return Username;
}
QString User::GetEncryptedPassword() {
    return EncryptedPassword;
}
QString User::GetFullName() {
    return FullName;
}
QString User::GetPhoneNumber() {
    return PhoneNumber;
}
bool User::GetIsActive() {
    return IsActive;
}
QString User::GetRole() {
    return Role;
}

QString User::GetUsernameById(int userID) {
    if (!User::GetActiveUser().hasPermission(Permission::manageUsers))
        return "Unknown";

    QSqlQuery query;
    query.prepare("SELECT Username FROM User WHERE UserID = :id");
    query.bindValue(":id", userID);
    if (query.exec() && query.next()) {
        return query.value("Username").toString();
    }
    return "Unknown";
}