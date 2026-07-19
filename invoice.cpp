#include "invoice.h"
#include "billableitem.h"
#include <QDateTime>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

Invoice::Invoice(int recordID, int patientID)
    : invoiceID(-1), recordID(recordID), patientID(patientID), totalAmount(0.0), isPaid(false) {
    dateIssued = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

Invoice::~Invoice() {
    qDeleteAll(items);
    items.clear();
}

void Invoice::addBillableItem(BillableItem* item) {
    items.append(item);
}

bool Invoice::initTable() {
    QSqlQuery query;
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS Invoices ("
        "InvoiceID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "RecordID INTEGER, "
        "DateIssued TEXT, "
        "TotalAmount REAL, "
        "IsPaid BOOLEAN, "
        "PatientID INTEGER, "
        "FOREIGN KEY(RecordID) REFERENCES MedicalRecords(RecordID), "
        "FOREIGN KEY(PatientID) REFERENCES Patients(ID))"
        );

    if (!success) {
        qDebug() << "Lỗi tạo bảng Invoices:" << query.lastError().text();
    }
    return success;
}

void Invoice::autoGenerateItems() {
    QSqlQuery query;

    // 1. Quét luồng Thuốc
    query.prepare("SELECT d.Name, d.Price, pd.Quantity, d.Unit "
                  "FROM Diagnoses dg "
                  "JOIN PrescriptionDetails pd ON dg.DiagnosisID = pd.DiagnosisID "
                  "JOIN Drugs d ON pd.DrugID = d.DrugID "
                  "WHERE dg.RecordID = :recordID");
    query.bindValue(":recordID", recordID);
    if (query.exec()) {
        while (query.next()) {
            addBillableItem(new DrugItem(
                query.value(0).toString(),
                query.value(1).toDouble(),
                query.value(2).toInt(),
                query.value(3).toString()
                ));
        }
    }

    // 2. Quét luồng Xét nghiệm
    query.prepare("SELECT l.Name, l.Price "
                  "FROM Diagnoses dg "
                  "JOIN LabTestOrders lo ON dg.DiagnosisID = lo.DiagnosisID "
                  "JOIN LabTests_Catalog l ON lo.LabTestID = l.LabTestID "
                  "WHERE dg.RecordID = :recordID");
    query.bindValue(":recordID", recordID);
    if (query.exec()) {
        while (query.next()) {
            addBillableItem(new LabTest(
                query.value(0).toString(),
                query.value(1).toDouble()
                ));
        }
    }

    // Tính lại tổng tiền
    totalAmount = 0;
    for (BillableItem* item : items) {
        totalAmount += item->CalculateCost();
    }
}

bool Invoice::save() {
    QSqlQuery query;
    if (invoiceID == -1) {
        query.prepare("INSERT INTO Invoices (RecordID, PatientID, DateIssued, TotalAmount, IsPaid) "
                      "VALUES (:recID, :patID, :date, :total, :paid)");
    } else {
        query.prepare("UPDATE Invoices SET TotalAmount=:total, IsPaid=:paid WHERE InvoiceID=:invID");
        query.bindValue(":invID", invoiceID);
    }

    query.bindValue(":recID", recordID);
    query.bindValue(":patID", patientID);
    query.bindValue(":date", dateIssued);
    query.bindValue(":total", totalAmount);
    query.bindValue(":paid", isPaid ? 1 : 0);

    if (!query.exec()) {
        qDebug() << "Lỗi lưu Invoice:" << query.lastError().text();
        return false;
    }

    if (invoiceID == -1) {
        invoiceID = query.lastInsertId().toInt();
    }

    // Lưu danh sách chi tiết
    for (BillableItem* item : items) {
        item->setInvoiceID(invoiceID);
        item->save();
    }
    return true;
}

int Invoice::getInvoiceID() const {
    return invoiceID;
}

void Invoice::markAsPaid() {
    isPaid = true;
    save();
}