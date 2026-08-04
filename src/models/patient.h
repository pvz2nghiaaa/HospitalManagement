#pragma once
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>
#include <QDate>
#include <QVariant>
#include <tuple>
#include "permission.h"
#include "user.h"

class Patient {
public:
    int ID = -1;
    QString FullName;
    QString Phone;
    QString BirthDate;
    QString Sex;
    QString Address;

    static bool initTable() {
        QSqlQuery q;
        return q.exec("CREATE TABLE IF NOT EXISTS Patients ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "FullName TEXT, Phone TEXT, BirthDate TEXT, Sex TEXT, Address TEXT)");
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


    static bool createPatient(QString fullName, QString phoneNo, QString birthDate, QString sex, QString address) {
        if (!User::GetActiveUser().hasPermission(Permission::createPatient)) {
            qDebug() << "Access Denied: Current user lacks permission to create patients.";
            return false;
        }
        QSqlQuery query;
        query.prepare("INSERT INTO Patients (FullName, Phone, BirthDate, Sex, Address) "
                      "VALUES (:name, :phoneNo, :dob, :sex, :addr)");
        query.bindValue(":name", fullName);
        query.bindValue(":phoneNo", phoneNo);
        query.bindValue(":dob", birthDate);
        query.bindValue(":sex", sex);
        query.bindValue(":addr", address);

        if (query.exec()) {
            qDebug() << "Patient registered successfully:" << fullName;
            return true;
        } else {
            qDebug() << query.lastError().text();
            return false;
        }
    }
    static QList<std::tuple<QString, QString, QString, QString, QString, QString>> findPatient(QString searchID, QString searchName, QString searchPhone)
    {
        QString baseSql = "SELECT ID, FullName, Phone, BirthDate, Sex, Address FROM Patients";
        QStringList conditions;

        if (!searchID.isEmpty()) {
            conditions.append("ID = :id");
        }
        if (!searchName.isEmpty()) {
            conditions.append("FullName LIKE :name");
        }
        if (!searchPhone.isEmpty()) {
            conditions.append("Phone = :phone");
        }
        if (!conditions.isEmpty()) {
            baseSql += " WHERE " + conditions.join(" AND ");
        }

        QSqlQuery query;
        query.prepare(baseSql);

        if (!searchID.isEmpty()) {
            query.bindValue(":id", searchID);
        }

        if (!searchName.isEmpty()) {
            // Adds % wildcards around input for flexible searching
            query.bindValue(":name", "%" + searchName + "%");
        }

        if (!searchPhone.isEmpty()) {
            query.bindValue(":phone", searchPhone);
        }

        QList<std::tuple<QString, QString, QString, QString, QString, QString>> lstPatient;
        if (!query.exec()) {
            qDebug() << "Query failed:" << query.lastError().text();
            return lstPatient;
        }

        while (query.next())
        {
            QString id    = query.value(0).toString();
            QString name  = query.value(1).toString();
            QString phone = query.value(2).toString();
            QString dob   = QDate::fromString(query.value(3).toString(), "yyyy-MM-dd").toString("dd-MM-yyyy");
            QString sex   = query.value(4).toString();
            QString addr  = query.value(5).toString();

            lstPatient.append(std::tuple<QString, QString, QString, QString, QString, QString>{id, name, phone, dob, sex, addr});
        }

        return lstPatient;
    }
    static bool updatePatient(QString id, QString name, QString phone, QString dob, QString sex, QString addr)
    {
        QSqlQuery query;
        query.prepare("UPDATE Patients SET FullName = :name, Phone = :phone, BirthDate = :dob, Sex = :sex, Address = :addr WHERE ID = :id");
        query.bindValue(":id", id);
        query.bindValue(":name", name);
        query.bindValue(":phone", phone);
        query.bindValue(":dob", dob);
        query.bindValue(":sex", sex);
        query.bindValue(":addr", addr);

        if (!query.exec())
        {
            qDebug() << "Query failed:" << query.lastError().text();
            return false;
        }

        return true;
    }

    static bool getPatientById(int id, Patient &outPatient) {
        QSqlQuery query;
        query.prepare("SELECT ID, FullName, Phone, BirthDate, Sex, Address FROM Patients WHERE ID = :id");
        query.bindValue(":id", id);
        if (query.exec() && query.next()) {
            outPatient.ID = query.value("ID").toInt();
            outPatient.FullName = query.value("FullName").toString();
            outPatient.Phone = query.value("Phone").toString();
            outPatient.BirthDate = query.value("BirthDate").toString();
            outPatient.Sex = query.value("Sex").toString();
            outPatient.Address = query.value("Address").toString();
            return true;
        }
        return false;
    }

};
