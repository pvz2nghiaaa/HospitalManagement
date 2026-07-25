#include "drug.h"

Drug::Drug(): drugID(-1),name(""),unit(""),price(0.0),stockQuantity(0){}

Drug::Drug(QString name, QString unit, double price)
    : drugID(-1), name(name), unit(unit), price(price), stockQuantity(0) {}

Drug::Drug(QString name, QString unit,double price, int stockQuantity)
    : drugID(-1),name(name),unit(unit),price(price),stockQuantity(stockQuantity){}

Drug::Drug(int drugID, QString name, QString unit, double price)
    : drugID(drugID), name(name), unit(unit), price(price) , stockQuantity(0){}


Drug::Drug(int drugID, QString name,QString unit, double price,int stockQuantity)
    : drugID(drugID),name(name),unit(unit),price(price),stockQuantity(stockQuantity){}

bool Drug::initTable() {
    static bool tableLoaded = false;
    if (!tableLoaded) {
        QSqlQuery query;
        tableLoaded = query.exec("CREATE TABLE IF NOT EXISTS Drugs ("
                                 "DrugID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "Name TEXT, "
                                 "Unit TEXT, "
                                 "Price REAL,"
                                 "StockQuantity INTEGER NOT NULL DEFAULT 0)"
            );
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
int Drug::getStockQuantity() const{return stockQuantity;}
// Setters
void Drug::setDrugID(int id) { drugID = id; }
void Drug::setName(const QString &nName) { name = nName; }
void Drug::setUnit(const QString &nUnit) { unit = nUnit; }
void Drug::setPrice(double nPrice) { price = nPrice; }
void Drug::setStockQuantity(int quantity){stockQuantity = quantity;}

// Fluent Setters
Drug& Drug::SetDrugID(int id) { drugID = id; return *this; }
Drug& Drug::SetName(const QString &nName) { name = nName; return *this; }
Drug& Drug::SetUnit(const QString &nUnit) { unit = nUnit; return *this; }
Drug& Drug::SetPrice(double nPrice) { price = nPrice; return *this; }
Drug& Drug::SetStockQuantity(int quantity){stockQuantity = quantity;return *this;}
