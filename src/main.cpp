#include "loginwindow.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QFileInfo>
#include <QDate>
#include <QFile>
#include <QDir>
#include <QTextStream>
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

void seedDiseasesFromCSV() {
    QSqlQuery checkQuery("SELECT COUNT(*) FROM Diseases");
    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        qDebug() << "=> Diseases table already populated. Skipping CSV import.";
        return;
    }

    // Solve database file path in the resources directory
    QString csvPath = QCoreApplication::applicationDirPath() + "/../../resources/ICD10codes.csv";
    QFile file(csvPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Warning: Failed to open ICD10 CSV file at:" << csvPath;
        return;
    }

    qDebug() << "=> Seeding database from ICD10codes.csv... Please wait.";
    QTextStream in(&file);
    QSqlDatabase::database().transaction();

    // Skip header line
    if (!in.atEnd()) {
        in.readLine();
    }

    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT OR IGNORE INTO Diseases (ConditionName, ICDCode) VALUES (:name, :code)");

    int count = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        int firstComma = line.indexOf(',');
        if (firstComma != -1) {
            QString code = line.left(firstComma).trimmed();
            QString name = line.mid(firstComma + 1).trimmed();
            if (name.startsWith('"') && name.endsWith('"')) {
                name = name.mid(1, name.length() - 2);
            }
            insertQuery.bindValue(":name", name);
            insertQuery.bindValue(":code", code);
            if (insertQuery.exec()) {
                count++;
            }
        }
    }
    QSqlDatabase::database().commit();
    qDebug() << "=> ICD database successfully seeded!" << count << "records imported.";
}

bool setupDatabase() {

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    // Set database path exactly to the resources folder relative to build folder
    QString dbPath = QCoreApplication::applicationDirPath() + "/../../resources/database.db";
    db.setDatabaseName(dbPath);

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
  
    // Create Diseases table and import CSV seeds
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS Diseases ("
               "ConditionName TEXT PRIMARY KEY, "
               "ICDCode TEXT NOT NULL)");
    seedDiseasesFromCSV();
  
    return true;
}





int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!setupDatabase()) {
        return -1;
    }
    Drug::initTable();
   
    qDebug() << "-------\n";
    LoginWindow w;
    w.show();
    return a.exec();
}