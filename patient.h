#pragma once
#include <QSqlQuery>
#include <QDebug>
#include <QString>
#include <QVariant>

class Patient {
public:
    int ID = -1;
    QString FullName;
    QString BirthDate;
    QString Sex;
    QString Address;
    // QString Phone;

    static bool initTable() {
        QSqlQuery q;
        return q.exec("CREATE TABLE IF NOT EXISTS Patients ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "FullName TEXT, BirthDate TEXT, Sex TEXT, Address TEXT)");
    }

    static int GetTotalPatients(){
        QSqlQuery query;
        query.prepare("SELECT COUNT(*) AS totalPatients FROM Patients");
        if (query.exec() && query.next()){
            return query.value("totalPatients").toInt();
        }
        qDebug() << "Failed to get total patients from Patients";
        return 0;
    }
};
