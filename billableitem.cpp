#include "billableitem.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

BillableItem::BillableItem(QString name, double price, int qty, QString unit, QString type)
    : itemID(-1), name(name), price(price), quantity(qty), unit(unit), itemType(type), invoiceID(-1) {}

BillableItem::~BillableItem() {}

void BillableItem::setInvoiceID(int id) {
    invoiceID = id;
}

bool BillableItem::save() {
    QSqlQuery query;
    if (itemID == -1) {
        query.prepare("INSERT INTO BillableItems (ItemType, Name, Price, Quantity, Unit, InvoiceID) "
                      "VALUES (:type, :name, :price, :qty, :unit, :invID)");
    } else {
        query.prepare("UPDATE BillableItems SET ItemType=:type, Name=:name, Price=:price, "
                      "Quantity=:qty, Unit=:unit, InvoiceID=:invID WHERE ItemID=:id");
        query.bindValue(":id", itemID);
    }
    query.bindValue(":type", itemType);
    query.bindValue(":name", name);
    query.bindValue(":price", price);
    query.bindValue(":qty", quantity);
    query.bindValue(":unit", unit);
    query.bindValue(":invID", invoiceID);

    if (query.exec()) {
        if (itemID == -1) itemID = query.lastInsertId().toInt();
        return true;
    }
    qDebug() << "Failed to save BillableItem:" << query.lastError().text();
    return false;
}

bool BillableItem::initTable() {
    QSqlQuery query;
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS BillableItems ("
        "ItemID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "ItemType TEXT, "
        "Name TEXT, "
        "Price REAL, "
        "Quantity INTEGER, "
        "Unit TEXT, "
        "InvoiceID INTEGER, "
        "FOREIGN KEY(InvoiceID) REFERENCES Invoices(InvoiceID))"
        );

    if (!success) {
        qDebug() << "Failed to create table BillableItems:" << query.lastError().text();
    }
    return success;
}
DrugItem::DrugItem(QString name, double price, int qty, QString unit)
    : BillableItem(name, price, qty, unit, "DRUG") {}

double DrugItem::CalculateCost() const {
    return price * quantity;
}


bool DrugItem::initTable() {
    QSqlQuery query;
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS Drugs ("
        "DrugID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "Name TEXT, Unit TEXT, Price REAL)"
        );
    if (!success) qDebug() << "Failed to create table Drugs:" << query.lastError().text();
    return success;
}

LabTest::LabTest(QString name, double price)
    : BillableItem(name, price, 1, "Time", "LABTEST") {}

double LabTest::CalculateCost() const {
    return price;
}

bool LabTest::initTable() {
    QSqlQuery query;
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS LabTests ("
        "LabTestID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "Name TEXT, Price REAL)"
        );
    if (!success) qDebug() << "Failed to create table LabTests:" << query.lastError().text();
    return success;
}
