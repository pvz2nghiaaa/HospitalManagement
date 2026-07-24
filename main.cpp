#include "loginwindow.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QFileInfo>
#include <QDate>

#include "user.h"
#include "permission.h"
#include "attendancelog.h"
#include "invoice.h"
#include "billableitem.h"
#include "billingmanager.h"

#include "patient.h"
#include "diagnosis.h"
#include "medicalrecord.h"
#include "prescriptiondetail.h"

#include "medicalrecord.h"
#include "drug.h"
#include "prescription.h"
#include "diagnosis.h"

bool setupDatabase() {
    // test
    QString dbName = "database.db";
    if (QFile::exists(dbName)) {
        if (QFile::remove(dbName)) {
            qDebug() << "=> Đã xoá file DB cũ để làm mới hoàn toàn!";
        } else {
            qDebug() << "=> Lỗi: Không thể xoá DB cũ (có thể đang bị phần mềm khác mở).";
        }
    }
    //

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");

    if (!db.open()) {
        qDebug() << "Database is not connected";
        qDebug() << "Error : " << db.lastError().text();
        return false;
    }

    qDebug() << "Database Is Connected";
    qDebug() << "VI TRI FILE DB THUC TE:" << QFileInfo(db.databaseName()).absoluteFilePath();

    User::initTable();
    Permission::initTable();
    AttendanceLog::initTable();
    DrugItem::initTable();
    LabTest::initTable();
    MedicalRecord::initTable();
    Drug::initTable();
    Prescription::initTable();
    Diagnosis::initTable();
    Patient::initTable();
    PrescriptionDetail::initTable();
    Invoice::initTable();
    BillableItem::initTable();
  
    return true;
}



void insertSampleData() {
    QSqlQuery query;
    // Insert Admin
    query.prepare("INSERT INTO User (Username, EncryptedPassword, FullName, PhoneNumber, Role, IsActive) "
                  "VALUES (:user, :pass, :name, :phone, :role, 1)");
    query.bindValue(":user", "admin");
    query.bindValue(":pass", "admin");
    query.bindValue(":name", "Default Admin");
    query.bindValue(":phone", "123456789");
    query.bindValue(":role", "Admin");
    if (query.exec()) {
        int adminId = query.lastInsertId().toInt();
        QList<Permission::Type> adminPerms = {
            Permission::viewLog, Permission::addLog, Permission::changePermission, Permission::manageUsers, Permission::viewRecord
        };
        for (Permission::Type p : adminPerms) {
            QSqlQuery permQuery;
            permQuery.prepare("INSERT INTO Permission (UserID, PermissionType) VALUES (:uid, :ptype)");
            permQuery.bindValue(":uid", adminId);
            permQuery.bindValue(":ptype", static_cast<int>(p));
            permQuery.exec();
        }
    }

    // Insert Doctor
    query.prepare("INSERT INTO User (Username, EncryptedPassword, FullName, PhoneNumber, Role, IsActive) "
                  "VALUES (:user, :pass, :name, :phone, :role, 1)");
    query.bindValue(":user", "doctor");
    query.bindValue(":pass", "doctor");
    query.bindValue(":name", "Dr. John Doe");
    query.bindValue(":phone", "987654321");
    query.bindValue(":role", "Doctor");
    if (query.exec()) {
        int doctorId = query.lastInsertId().toInt();
        QList<Permission::Type> docPerms = {
            Permission::createRecord, Permission::viewRecord, Permission::editRecord, Permission::manageDrugs
        };
        for (Permission::Type p : docPerms) {
            QSqlQuery permQuery;
            permQuery.prepare("INSERT INTO Permission (UserID, PermissionType) VALUES (:uid, :ptype)");
            permQuery.bindValue(":uid", doctorId);
            permQuery.bindValue(":ptype", static_cast<int>(p));
            permQuery.exec();
        }
    }

    // Insert Receptionist
    query.prepare("INSERT INTO User (Username, EncryptedPassword, FullName, PhoneNumber, Role, IsActive) "
                  "VALUES (:user, :pass, :name, :phone, :role, 1)");
    query.bindValue(":user", "receptionist");
    query.bindValue(":pass", "receptionist");
    query.bindValue(":name", "Sarah Smith");
    query.bindValue(":phone", "555123456");
    query.bindValue(":role", "Receptionist");
    if (query.exec()) {
        int recId = query.lastInsertId().toInt();
        QList<Permission::Type> recPerms = {
            Permission::createPatient, Permission::editPatient, Permission::viewRecord, Permission::createInvoice, Permission::viewInvoice
        };
        for (Permission::Type p : recPerms) {
            QSqlQuery permQuery;
            permQuery.prepare("INSERT INTO Permission (UserID, PermissionType) VALUES (:uid, :ptype)");
            permQuery.bindValue(":uid", recId);
            permQuery.bindValue(":ptype", static_cast<int>(p));
            permQuery.exec();
        }
    }
    qDebug() << "=> Sample users and permissions inserted successfully!";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!setupDatabase()) {
        return -1;
    }
    insertSampleData();
    qDebug() << "-------\n";
    LoginWindow w;
    w.show();

    return a.exec();
}