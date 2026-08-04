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
    /*QString dbName = "database.db";
    if (QFile::exists(dbName)) {
        if (QFile::remove(dbName)) {
            qDebug() << "=> Đã xoá file DB cũ để làm mới hoàn toàn!";
        } else {
            qDebug() << "=> Lỗi: Không thể xoá DB cũ (có thể đang bị phần mềm khác mở).";
        }
    }*/
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
    QSqlDatabase::database().transaction();
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
            Permission::viewLog, Permission::addLog,
            Permission::changePermission, Permission::manageUsers,
            Permission::viewRecord, Permission::createPatient
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
        int doctorId = query.lastInsertId().toInt(); // Lấy ID của Doctor vừa tạo

        // Cấp quyền cho Doctor
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

        // ---> BỔ SUNG: THÊM DỮ LIỆU ATTENDANCE LOG MẪU CHO BÁC SĨ NÀY <---
        QList<QString> sampleDates = {"2026-07-20", "2026-07-21", "2026-07-22", "2026-07-23", "2026-07-24", "2026-07-25"};
        QList<int> sampleStatus = {1, 1, 0, 1, 1, 1}; // 1 = Present (Có mặt), 0 = Absent (Vắng)

        for (int i = 0; i < sampleDates.size(); ++i) {
            QSqlQuery logQuery;
            logQuery.prepare("INSERT INTO AttendanceLogs (Date, IsPresent, EmployeeID) VALUES (:date, :isPresent, :empId)");
            logQuery.bindValue(":date", sampleDates[i]);
            logQuery.bindValue(":isPresent", sampleStatus[i]);
            logQuery.bindValue(":empId", doctorId); // Gắn chính xác với ID của bác sĩ

            if (!logQuery.exec()) {
                qDebug() << "Failed to insert sample attendance log:" << logQuery.lastError().text();
            }
        }
    }

    // Insert Receptionist (Sẽ tự động nhận UserID = 3)
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

    // Insert Patient
    query.prepare("INSERT INTO Patients (FullName, Phone, BirthDate, Sex, Address) "
                  "VALUES (:name, :phoneNo, :dob, :sex, :addr)");
    query.bindValue(":name", "David Smith");
    query.bindValue(":phoneNo", "7412589630");
    query.bindValue(":dob", "06-09-2012");
    query.bindValue(":sex", "Male");
    query.bindValue(":addr", "123 A Street");
    if (query.exec()) {
        qDebug() << "Patient 1 registered successfully";
    } else {
        qDebug() << "Failed to register patient:" << query.lastError().text();
    }
    query.prepare("INSERT INTO Patients (FullName, Phone, BirthDate, Sex, Address) "
                  "VALUES (:name, :phoneNo, :dob, :sex, :addr)");
    query.bindValue(":name", "Emilia Rose");
    query.bindValue(":phoneNo", "4632105789");
    query.bindValue(":dob", "17-12-1994");
    query.bindValue(":sex", "Female");
    query.bindValue(":addr", "234 B Street");
    if (query.exec()) {
        qDebug() << "Patient 2 registered successfully";
    } else {
        qDebug() << "Failed to register patient:" << query.lastError().text();
    }
    query.prepare("INSERT INTO Patients (FullName, Phone, BirthDate, Sex, Address) "
                  "VALUES (:name, :phoneNo, :dob, :sex, :addr)");
    query.bindValue(":name", "David Bob");
    query.bindValue(":phoneNo", "9874152630");
    query.bindValue(":dob", "23-09-1985");
    query.bindValue(":sex", "Male");
    query.bindValue(":addr", "471 C Street");
    if (query.exec()) {
        qDebug() << "Patient 3 registered successfully";
    } else {
        qDebug() << "Failed to register patient:" << query.lastError().text();
    }

    // Insert Explicitly Indexed Patients for Medical Records
    if (!query.exec("INSERT INTO Patients (ID, FullName, Sex, BirthDate, Address) "
                    "VALUES (201, 'Nguyen Van A', 'Male', '1990-05-15', 'Q1, TP.HCM')")) {
        qDebug() << "LỖI BỆNH NHÂN 201:" << query.lastError().text();
    }
    if (!query.exec("INSERT INTO Patients (ID, FullName, Sex, BirthDate, Address) "
                    "VALUES (202, 'Tran Thi B', 'Female', '1985-10-20', 'Q3, TP.HCM')")) {
        qDebug() << "LỖI BỆNH NHÂN 202:" << query.lastError().text();
    }

    // Thêm Bệnh án (Bảng MedicalRecords - Giữ nguyên)
    if (!query.exec("INSERT INTO MedicalRecords (RecordID, PatientID, Date, IsComplete) "
                    "VALUES (301, 201, '27-07-2026', 1)")) {
        qDebug() << "LỖI BỆNH ÁN 301:" << query.lastError().text();
    }
    if (!query.exec("INSERT INTO MedicalRecords (RecordID, PatientID, Date, IsComplete) "
                    "VALUES (302, 202, '28-07-2026', 0)")) {
        qDebug() << "LỖI BỆNH ÁN 302:" << query.lastError().text();
    }


    // Thêm Chẩn đoán (Bảng Diagnoses - Đổi tên bảng và đổi cột Note thành ICDCode)
    if (!query.exec("INSERT INTO Diagnoses (DiagnosisID, RecordID, DoctorID, ConditionName, Severity, ICDCode) "
                    "VALUES (401, 301, 2, 'Influenza', 'Moderate', 'J09')")) {
        qDebug() << "LỖI CHẨN ĐOÁN 401:" << query.lastError().text();
    }

    // Thêm Danh mục Thuốc (Bảng Drugs)
    if (!query.exec("INSERT INTO Drugs (DrugID, Name, Unit, Price) "
                    "VALUES (501, 'Paracetamol 500mg', 'Viên', 2000)")) {
        qDebug() << "LỖI THUỐC 501:" << query.lastError().text();
    }
    if (!query.exec("INSERT INTO Drugs (DrugID, Name, Unit, Price) "
                    "VALUES (502, 'Amoxicillin 250mg', 'Viên', 5000)")) {
        qDebug() << "LỖI THUỐC 502:" << query.lastError().text();
    }
    if (!query.exec("INSERT INTO Drugs (DrugID, Name, Unit, Price) "
                    "VALUES (503, 'Oresol', 'Gói', 3000)")) {
        qDebug() << "LỖI THUỐC 503:" << query.lastError().text();
    }

    // Thêm Đơn thuốc (Bảng Prescriptions - Đổi PrescriptionID thành DetailID)
    if (!query.exec("INSERT INTO Prescriptions (DetailID, DiagnosisID, DrugID, Quantity, Note) "
                    "VALUES (601, 401, 501, 10, 'Uống 2 viên/ngày sau ăn')")) {
        qDebug() << "LỖI ĐƠN THUỐC 601:" << query.lastError().text();
    }
    if (!query.exec("INSERT INTO Prescriptions (DetailID, DiagnosisID, DrugID, Quantity, Note) "
                    "VALUES (602, 401, 503, 5, 'Pha 1 gói với 200ml nước')")) {
        qDebug() << "LỖI ĐƠN THUỐC 602:" << query.lastError().text();
    }

    // 1. Thêm Bệnh án mới cho Bệnh nhân 201 (RecordID: 303)
    if (!query.exec("INSERT INTO MedicalRecords (RecordID, PatientID, Date, IsComplete) "
                    "VALUES (303, 201, '28-07-2026', 0)")) {
        qDebug() << "LỖI BỆNH ÁN 303:" << query.lastError().text();
    }

    // 2. Thêm Chẩn đoán mới liên kết với Bệnh án 303 (DiagnosisID: 402)
    if (!query.exec("INSERT INTO Diagnoses (DiagnosisID, RecordID, DoctorID, ConditionName, Severity, ICDCode) "
                    "VALUES (402, 303, 1, 'Acute Gastritis', 'Mild', 'K29.1')")) {
        qDebug() << "LỖI CHẨN ĐOÁN 402:" << query.lastError().text();
    }

    // 3. Thêm Thuốc mới vào danh mục nếu cần (DrugID: 504)
    if (!query.exec("INSERT INTO Drugs (DrugID, Name, Unit, Price) "
                    "VALUES (504, 'Omeprazole 20mg', 'Viên', 4000)")) {
        qDebug() << "LỖI THUỐC 504:" << query.lastError().text();
    }

    // 4. Kê đơn thuốc cho Chẩn đoán 402 (DetailID: 603)
    if (!query.exec("INSERT INTO Prescriptions (DetailID, DiagnosisID, DrugID, Quantity, Note) "
                    "VALUES (603, 402, 504, 14, 'Uống 1 viên trước khi ăn sáng 30 phút')")) {
        qDebug() << "LỖI ĐƠN THUỐC 603:" << query.lastError().text();
    }

    qDebug() << "=> Sample users, permissions, and attendance logs inserted successfully!";
    // Insert Sample Drugs
    query.prepare(
        "INSERT INTO Drugs "
        "(Name, Unit, Price, StockQuantity) "
        "VALUES (:name, :unit, :price, :stock)"
    );

    query.bindValue(":name", "Paracetamol");
    query.bindValue(":unit", "Tablet");
    query.bindValue(":price", 5000);
    query.bindValue(":stock", 200);

    if (query.exec())
    {
        Drug::AddHistory(
            "Paracetamol",
            "ADD",
            200
        );

        qDebug() << "Paracetamol inserted successfully";
    }
    else
    {
        qDebug()
            << "Failed to insert Paracetamol:"
            << query.lastError().text();
    }


    query.prepare(
        "INSERT INTO Drugs "
        "(Name, Unit, Price, StockQuantity) "
        "VALUES (:name, :unit, :price, :stock)"
    );

    query.bindValue(":name", "Amoxicillin");
    query.bindValue(":unit", "Capsule");
    query.bindValue(":price", 12000);
    query.bindValue(":stock", 150);

    if (query.exec())
    {
        Drug::AddHistory(
            "Amoxicillin",
            "ADD",
            150
        );
    }
    else
    {
        qDebug()
            << "Failed to insert Amoxicillin:"
            << query.lastError().text();
    }


    query.prepare(
        "INSERT INTO Drugs "
        "(Name, Unit, Price, StockQuantity) "
        "VALUES (:name, :unit, :price, :stock)"
    );

    query.bindValue(":name", "Vitamin C");
    query.bindValue(":unit", "Tablet");
    query.bindValue(":price", 3000);
    query.bindValue(":stock", 8);

    if (query.exec())
    {
        Drug::AddHistory(
            "Vitamin C",
            "ADD",
            8
        );
    }
    else
    {
        qDebug()
            << "Failed to insert Vitamin C:"
            << query.lastError().text();
    }
    QSqlDatabase::database().commit();

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!setupDatabase()) {
        return -1;
    }
    Drug::initTable();
    insertSampleData();
   
    qDebug() << "-------\n";
    LoginWindow w;
    w.show();
    return a.exec();
}