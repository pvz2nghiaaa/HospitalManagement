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
    Invoice::initTable();
    BillableItem::initTable();
    DrugItem::initTable();
    LabTest::initTable();
    return true;
}
bool initDatabase() {
    QSqlQuery query;

    // Bật ràng buộc khóa ngoại cho SQLite
    query.exec("PRAGMA foreign_keys = ON;");

    // --- CỤM 1: NHÂN SỰ ---
    query.exec("CREATE TABLE IF NOT EXISTS Users ("
               "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "Username TEXT, Password TEXT, Role TEXT, "
               "Name TEXT, PhoneNumber TEXT, isActive BOOLEAN)");

    query.exec("CREATE TABLE IF NOT EXISTS Permissions ("
               "PermissionID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "PermissionType INTEGER, UserID INTEGER, "
               "FOREIGN KEY(UserID) REFERENCES Users(ID), "
               "CONSTRAINT uq_permissions_user_permission UNIQUE(UserID, PermissionType))");

    query.exec("CREATE TABLE IF NOT EXISTS AttendanceLogs ("
               "LogID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "Date TEXT, Time TEXT, IsPresent INTEGER, EmployeeID INTEGER, "
               "FOREIGN KEY(EmployeeID) REFERENCES Users(ID))");

    // --- CỤM 2: BỆNH ÁN ---
    query.exec("CREATE TABLE IF NOT EXISTS Patients ("
               "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "FullName TEXT, BirthDate TEXT, Sex TEXT, Address TEXT)");

    query.exec("CREATE TABLE IF NOT EXISTS MedicalRecords ("
               "RecordID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "Date TEXT, IsComplete BOOLEAN, PatientID INTEGER, "
               "FOREIGN KEY(PatientID) REFERENCES Patients(ID))");

    query.exec("CREATE TABLE IF NOT EXISTS Diagnoses ("
               "DiagnosisID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "ConditionName TEXT, ICDCode TEXT, Severity TEXT, DateDiagnosed TEXT, "
               "DoctorID INTEGER, RecordID INTEGER, "
               "FOREIGN KEY(DoctorID) REFERENCES Users(ID), "
               "FOREIGN KEY(RecordID) REFERENCES MedicalRecords(RecordID))");

    // --- CỤM 3: MASTER DATA (THUỐC & XÉT NGHIỆM) ---
    query.exec("CREATE TABLE IF NOT EXISTS Drugs ("
               "DrugID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "Name TEXT, Unit TEXT, Price REAL)");

    query.exec("CREATE TABLE IF NOT EXISTS LabTests_Catalog ("
               "LabTestID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "Name TEXT, Price REAL)");

    query.exec("CREATE TABLE IF NOT EXISTS PrescriptionDetails ("
               "DetailID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "Quantity INTEGER, Note TEXT, DrugID INTEGER, DiagnosisID INTEGER, "
               "FOREIGN KEY(DrugID) REFERENCES Drugs(DrugID), "
               "FOREIGN KEY(DiagnosisID) REFERENCES Diagnoses(DiagnosisID), "
               "CONSTRAINT uq_prescription UNIQUE(DiagnosisID, DrugID))");

    query.exec("CREATE TABLE IF NOT EXISTS LabTestOrders ("
               "OrderID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "DiagnosisID INTEGER, LabTestID INTEGER, Result TEXT, Note TEXT, "
               "FOREIGN KEY(DiagnosisID) REFERENCES Diagnoses(DiagnosisID), "
               "FOREIGN KEY(LabTestID) REFERENCES LabTests_Catalog(LabTestID), "
               "CONSTRAINT uq_labtest_diagnosis UNIQUE(DiagnosisID, LabTestID))");

    // --- CỤM 4: TÍNH TIỀN ---
    query.exec("CREATE TABLE IF NOT EXISTS Invoices ("
               "InvoiceID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "RecordID INTEGER, DateIssued TEXT, TotalAmount REAL, IsPaid BOOLEAN, PatientID INTEGER, "
               "FOREIGN KEY(RecordID) REFERENCES MedicalRecords(RecordID), "
               "FOREIGN KEY(PatientID) REFERENCES Patients(ID))");

    query.exec("CREATE TABLE IF NOT EXISTS BillableItems ("
               "ItemID INTEGER PRIMARY KEY AUTOINCREMENT, "
               "ItemType TEXT, Name TEXT, Price REAL, Quantity INTEGER, Unit TEXT, InvoiceID INTEGER, "
               "FOREIGN KEY(InvoiceID) REFERENCES Invoices(InvoiceID))");

    return true;
}

// ==============================================================
// 2. HÀM BƠM DỮ LIỆU GIẢ (Mock Data)
// ==============================================================
void insertMockData() {
    QSqlQuery q;

    // Master Data
    q.exec("INSERT INTO Drugs (DrugID, Name, Unit, Price) VALUES (1, 'Paracetamol', 'Viên', 5000)");
    q.exec("INSERT INTO Drugs (DrugID, Name, Unit, Price) VALUES (2, 'Amoxicillin', 'Viên', 8000)");
    q.exec("INSERT INTO LabTests_Catalog (LabTestID, Name, Price) VALUES (1, 'Xét nghiệm Máu', 150000)");

    // Con người & Bệnh án
    q.exec("INSERT INTO Users (ID, Name, Role) VALUES (1, 'Bác sĩ Hưng', 'Doctor')");
    q.exec("INSERT INTO Patients (ID, FullName) VALUES (1, 'Trần Nguyễn Anh Khoa')");
    q.exec("INSERT INTO MedicalRecords (RecordID, Date, IsComplete, PatientID) VALUES (1, '2026-04-20', 1, 1)");

    // Khám & Kê đơn
    q.exec("INSERT INTO Diagnoses (DiagnosisID, ConditionName, DoctorID, RecordID) VALUES (1, 'Sốt siêu vi', 1, 1)");
    q.exec("INSERT INTO PrescriptionDetails (Quantity, DrugID, DiagnosisID) VALUES (2, 1, 1)"); // 2 Paracetamol
    q.exec("INSERT INTO PrescriptionDetails (Quantity, DrugID, DiagnosisID) VALUES (10, 2, 1)"); // 10 Amoxicillin
    q.exec("INSERT INTO LabTestOrders (LabTestID, DiagnosisID) VALUES (1, 1)"); // 1 Xét nghiệm máu
}

// ==============================================================
// 3. HÀM IN KẾT QUẢ TỪ DATABASE
// ==============================================================
void printInvoiceDatabase() {
    qDebug() << "\n--- TRUY VẤN DATABASE ---";
    QSqlQuery q;

    q.exec("SELECT InvoiceID, TotalAmount, IsPaid FROM Invoices");
    while (q.next()) {
        qDebug() << "[INVOICE] ID:" << q.value(0).toInt()
        << "| Tổng tiền:" << q.value(1).toDouble()
        << "| Trạng thái thanh toán:" << (q.value(2).toBool() ? "Đã trả" : "Chưa trả");
    }

    qDebug() << "\n--- CHI TIẾT CÁC KHOẢN PHÍ TRONG HÓA ĐƠN ---";
    q.exec("SELECT ItemType, Name, Quantity, Unit, Price, InvoiceID FROM BillableItems");
    while (q.next()) {
        qDebug() << QString("[%1] %2 - SL: %3 %4 - Đơn giá: %5 - Thuộc Invoice ID: %6")
                        .arg(q.value(0).toString(), 8)
                        .arg(q.value(1).toString(), -15)
                        .arg(q.value(2).toInt(), 2)
                        .arg(q.value(3).toString(), -5)
                        .arg(q.value(4).toDouble(), 7, 'f', 0)
                        .arg(q.value(5).toInt());
    }
}
void testBillingProcess() {
    qDebug() << "\n=============================================";
    qDebug() << "      BẮT ĐẦU TEST HỆ THỐNG TÍNH TIỀN      ";
    qDebug() << "=============================================";

    insertMockData(); // Bơm bệnh án, thuốc, xét nghiệm giả vào

    // 2. Chạy logic nghiệp vụ
    qDebug() << "\n[2/3] Chạy BillingManager để tạo hóa đơn...";
    BillingManager billingMgr;

    // Giả sử bệnh án số 1 của bệnh nhân số 1 vừa khám xong
    int testRecordID = 1;
    int testPatientID = 1;

    Invoice* myInvoice = billingMgr.CreateInvoice(testRecordID, testPatientID);

    // 3. Kiểm chứng kết quả
    qDebug() << "\n[3/3] Kết quả thực thi:";
    if (myInvoice != nullptr) {
        qDebug() << "  -> Thành công! Tạo Hóa đơn mang ID:" << myInvoice->getInvoiceID();

        // Thử thao tác thanh toán
        myInvoice->markAsPaid();
        qDebug() << "  -> Đã cập nhật trạng thái hóa đơn thành: Đã thanh toán (IsPaid = true)";

        // Giải phóng con trỏ
        delete myInvoice;
    } else {
        qDebug() << "  -> Thất bại! Không thể tạo hóa đơn.";
    }

    // In toàn bộ DB ra xem hàm save() có lưu đúng bảng Invoices và BillableItems không
    printInvoiceDatabase();

    qDebug() << "================ KẾT THÚC ===================\n";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!setupDatabase()) {
        return -1;
    }
    // testAttendanceLog();
    testBillingProcess();
    qDebug() << "-------\n";
    // LoginWindow w;
    // w.show();

    return a.exec();
}