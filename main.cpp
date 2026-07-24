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



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!setupDatabase()) {
        return -1;
    }
    // testAttendanceLog();
    // testBillingProcess();
    qDebug() << "-------\n";
    LoginWindow w;
    w.show();

    return a.exec();
}