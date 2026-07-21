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

void insertMockData() {
    QSqlQuery q;

    // Hàm lambda hỗ trợ chạy và báo lỗi chi tiết nếu có
    auto runQuery = [&](const QString& sql) {
        if (!q.exec(sql)) {
            qDebug() << "LỖI INSERT:" << q.lastError().text() << "\nQuery:" << sql;
        }
    };

    qDebug() << "--- Đang bơm dữ liệu giả (Mock Data) ---";

    // ==========================================
    // 1. DỮ LIỆU NHÂN SỰ (Bảng Users)
    // ==========================================
    // Lưu ý: Cột UserID, Username, EncryptedPassword, FullName, PhoneNumber, IsActive
    runQuery("INSERT INTO User (UserID, Username, EncryptedPassword, FullName, PhoneNumber, IsActive) "
             "VALUES (1, 'admin', 'admin123', 'Quản trị viên', '0123456789', 1)");

    runQuery("INSERT INTO User (UserID, Username, EncryptedPassword, FullName, PhoneNumber, IsActive) "
             "VALUES (2, 'bshung', '123456', 'Bác sĩ Hưng', '0987654321', 1)");

    // ==========================================
    // 2. MASTER DATA (Thuốc & Xét nghiệm)
    // ==========================================
    runQuery("INSERT INTO Drugs (DrugID, Name, Unit, Price) "
             "VALUES (1, 'Paracetamol', 'Viên', 5000)");

    runQuery("INSERT INTO Drugs (DrugID, Name, Unit, Price) "
             "VALUES (2, 'Amoxicillin', 'Viên', 8000)");

    runQuery("INSERT INTO LabTests (LabTestID, Name, Price) "
             "VALUES (1, 'Xét nghiệm Máu', 150000)");

    // ==========================================
    // 3. BỆNH NHÂN & BỆNH ÁN
    // ==========================================
    runQuery("INSERT INTO Patients (ID, FullName, BirthDate, Sex, Address) "
             "VALUES (1, 'Trần Nguyễn Anh Khoa', '2005-01-01', 'Nam', 'Ký túc xá ĐHQG')");

    // Khởi tạo bệnh án (RecordID = 1) cho bệnh nhân ID = 1
    runQuery("INSERT INTO MedicalRecords (RecordID, Date, IsComplete, PatientID) "
             "VALUES (1, '2026-04-20', 1, 1)");

    // ==========================================
    // 4. KHÁM BỆNH, XÉT NGHIỆM & KÊ ĐƠN
    // ==========================================
    // Chẩn đoán gắn với Bác sĩ (DoctorID = 2), Bệnh án (RecordID = 1)
    // VÀ gắn kèm Xét nghiệm luôn (LabTestID = 1) do ta đã gộp bảng
    runQuery("INSERT INTO Diagnoses (DiagnosisID, ConditionName, DoctorID, RecordID, LabTestID, LabTestResult) "
             "VALUES (1, 'Sốt siêu vi', 2, 1, 1, 'Bạch cầu tăng nhẹ')");

    // Kê đơn thuốc gắn trực tiếp với Bệnh án (RecordID = 1)
    runQuery("INSERT INTO PrescriptionDetails (Quantity, Note, DrugID, RecordID) "
             "VALUES (2, 'Uống sáng chiều', 1, 1)"); // 2 viên Paracetamol

    runQuery("INSERT INTO PrescriptionDetails (Quantity, Note, DrugID, RecordID) "
             "VALUES (10, 'Uống sau ăn', 2, 1)");    // 10 viên Amoxicillin

    qDebug() << "--- Bơm dữ liệu giả hoàn tất ---";
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

    Patient::initTable();
    MedicalRecord::initTable();
    Diagnosis::initTable();
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
    testBillingProcess();
    qDebug() << "-------\n";
    // LoginWindow w;
    // w.show();

    return a.exec();
}