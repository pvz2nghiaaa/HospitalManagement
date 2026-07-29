#include "drug.h"

Drug::Drug() : drugID(-1), price(0.0) {}

Drug::Drug(QString name, QString unit, double price)
    : drugID(-1), name(name), unit(unit), price(price) {}

Drug::Drug(int drugID, QString name, QString unit, double price)
    : drugID(drugID), name(name), unit(unit), price(price) {}

bool Drug::initTable() {
    static bool tableLoaded = false;
    if (!tableLoaded) {
        QSqlQuery query;
        tableLoaded = query.exec("CREATE TABLE IF NOT EXISTS Drugs ("
                                 "DrugID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "Name TEXT, "
                                 "Unit TEXT, "
                                 "Price REAL)");
        if (!tableLoaded) {
            qDebug() << "Failed to create Drugs table:" << query.lastError().text();
        } else {
            qDebug() << "Drugs table is initialized";
        }
        return tableLoaded;
    }
    return true;
}

// Getters
int Drug::getDrugID() const { return drugID; }
QString Drug::getName() const { return name; }
QString Drug::getUnit() const { return unit; }
double Drug::getPrice() const { return price; }

// Setters
void Drug::setDrugID(int id) { drugID = id; }
void Drug::setName(const QString &nName) { name = nName; }
void Drug::setUnit(const QString &nUnit) { unit = nUnit; }
void Drug::setPrice(double nPrice) { price = nPrice; }

// Fluent Setters
Drug& Drug::SetDrugID(int id) { drugID = id; return *this; }
Drug& Drug::SetName(const QString &nName) { name = nName; return *this; }
Drug& Drug::SetUnit(const QString &nUnit) { unit = nUnit; return *this; }
Drug& Drug::SetPrice(double nPrice) { price = nPrice; return *this; }
