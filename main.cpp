#include "loginwindow.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QFileInfo>

#include "user.h"
#include "permission.h"
#include "attendancelog.h"
#include "medicalrecord.h"
#include "drug.h"
#include "prescription.h"
#include "diagnosis.h"

bool setupDatabase() {
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
    MedicalRecord::initTable();
    Drug::initTable();
    Prescription::initTable();
    Diagnosis::initTable();

    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!setupDatabase()) {
        return -1;
    }

    LoginWindow w;
    w.show();

    return a.exec();
}