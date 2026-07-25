#include <QSqlQuery>
#include <QDebug>
#include "permission.h"
#include "user.h"

class Patient {
public:
    static bool initTable() {
        QSqlQuery q;
        return q.exec("CREATE TABLE IF NOT EXISTS Patients ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "FullName TEXT, BirthDate TEXT, Sex TEXT, Address TEXT)");
    }
    static int GetTotalPatients(){
        if (!User::GetActiveUser().hasPermission(Permission::viewLog)){
            qDebug() << "User does not have permission to view patient info";
            return 0;
        }
        QSqlQuery query;
        query.prepare("SELECT COUNT(*) AS totalPatients FROM Patients");
        if (query.exec() && query.next()){
            return query.value("totalPatients").toInt();
        }
        qDebug() << "Failed to get total patients from Patients";
        return 0;
    }
    static bool createPatient(QString fullName, QString birthDate, QString sex, QString address) {
        if (!User::GetActiveUser().hasPermission(Permission::createPatient)) {
            qDebug() << "Access Denied: Current user lacks permission to create patients.";
            return false;
        }
        QSqlQuery query;
        query.prepare("INSERT INTO Patients (FullName, BirthDate, Sex, Address) "
                      "VALUES (:name, :dob, :sex, :addr)");
        query.bindValue(":name", fullName);
        query.bindValue(":dob", birthDate);
        query.bindValue(":sex", sex);
        query.bindValue(":addr", address);

        if (query.exec()) {
            qDebug() << "Patient registered successfully:" << fullName;
            return true;
        } else {
            qDebug() << "Failed to register patient:" << query.lastError().text();
            return false;
        }
    }
};