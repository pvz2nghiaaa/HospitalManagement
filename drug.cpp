#include "drug.h"

Drug::Drug() : drugID(-1), name(""), unit(""), price(0.0), stockQuantity(0) {}

Drug::Drug(QString name, QString unit, double price)
    : drugID(-1), name(name), unit(unit), price(price), stockQuantity(0) {
}

Drug::Drug(QString name, QString unit, double price, int stockQuantity)
    : drugID(-1), name(name), unit(unit), price(price), stockQuantity(stockQuantity) {
}

Drug::Drug(int drugID, QString name, QString unit, double price)
    : drugID(drugID), name(name), unit(unit), price(price), stockQuantity(0) {
}


Drug::Drug(int drugID, QString name, QString unit, double price, int stockQuantity)
    : drugID(drugID), name(name), unit(unit), price(price), stockQuantity(stockQuantity) {
}
bool Drug::initTable()
{
    static bool tableLoaded = false;

    if (!tableLoaded)
    {
        QSqlQuery query;


        // ======================
        // Drugs table
        // ======================

        bool drugTable = query.exec(
            "CREATE TABLE IF NOT EXISTS Drugs ("
            "DrugID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "Name TEXT,"
            "Unit TEXT,"
            "Price REAL,"
            "StockQuantity INTEGER NOT NULL DEFAULT 0"
            ")"
        );


        if (!drugTable)
        {
            qDebug()
                << "Create Drugs error:"
                << query.lastError().text();

            return false;
        }



        // ======================
        // Drug Stock History
        // ======================

        bool historyTable = query.exec(
            "CREATE TABLE IF NOT EXISTS DrugStockHistory ("
            "HistoryID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "DrugID INTEGER,"
            "Action TEXT,"
            "OldQuantity INTEGER,"
            "NewQuantity INTEGER,"
            "Time TEXT,"
            "Operator TEXT,"
            "FOREIGN KEY(DrugID) REFERENCES Drugs(DrugID)"
            ")"
        );


        if (!historyTable)
        {
            qDebug()
                << "Create History error:"
                << query.lastError().text();

            return false;
        }



        tableLoaded = true;


        qDebug()
            << "Drugs and History tables initialized";
    }


    return true;
}


// Getters
int Drug::getDrugID() const { return drugID; }
QString Drug::getName() const { return name; }
QString Drug::getUnit() const { return unit; }
double Drug::getPrice() const { return price; }
int Drug::getStockQuantity() const { return stockQuantity; }
// Setters
void Drug::setDrugID(int id) { drugID = id; }
void Drug::setName(const QString& nName) { name = nName; }
void Drug::setUnit(const QString& nUnit) { unit = nUnit; }
void Drug::setPrice(double nPrice) { price = nPrice; }
void Drug::setStockQuantity(int quantity) { stockQuantity = quantity; }

// Fluent Setters
Drug& Drug::SetDrugID(int id) { drugID = id; return *this; }
Drug& Drug::SetName(const QString& nName) { name = nName; return *this; }
Drug& Drug::SetUnit(const QString& nUnit) { unit = nUnit; return *this; }
Drug& Drug::SetPrice(double nPrice) { price = nPrice; return *this; }
Drug& Drug::SetStockQuantity(int quantity) { stockQuantity = quantity; return *this; }


QString Drug::GetDrugStatus(int stockQuantity)
{
    if (stockQuantity > 10)
        return "Available";

    if (stockQuantity > 0)
        return "Low Stock";

    return "Out of Stock";
}

//Search

QList<Drug> Drug::SearchDrugsBy(const QString& nameOrID,
    const QString& stockStatus)
{
    QList<Drug> list;

    QSqlQuery query;

    QString sql =
        "SELECT DrugID, Name, Unit, Price, StockQuantity "
        "FROM Drugs WHERE 1=1 ";

    QString keyword = nameOrID.trimmed();

    if (!keyword.isEmpty())
    {
        sql +=
            "AND (CAST(DrugID AS TEXT) LIKE :keyword "
            "OR Name LIKE :keyword) ";
    }

    if (stockStatus == "Available")
        sql += "AND StockQuantity>10 ";

    else if (stockStatus == "Low Stock")
        sql += "AND StockQuantity>0 AND StockQuantity<=10 ";

    else if (stockStatus == "Out of Stock")
        sql += "AND StockQuantity=0 ";

    sql += "ORDER BY DrugID";

    query.prepare(sql);

    if (!keyword.isEmpty())
        query.bindValue(":keyword", "%" + keyword + "%");

    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return list;
    }

    while (query.next())
    {
        Drug drug;

        drug.setDrugID(query.value("DrugID").toInt());
        drug.setName(query.value("Name").toString());
        drug.setUnit(query.value("Unit").toString());
        drug.setPrice(query.value("Price").toDouble());
        drug.setStockQuantity(query.value("StockQuantity").toInt());

        list.push_back(drug);
    }

    return list;
}

//Update/Add druvg
bool Drug::AddNewDrug(
    const QString& name,
    const QString& unit,
    double price,
    int stockQuantity)
{
    if (name.trimmed().isEmpty())
        return false;

    if (unit.trimmed().isEmpty())
        return false;

    if (price < 0)
        return false;

    if (stockQuantity < 0)
        return false;

    QSqlQuery query;

    query.prepare(
        "INSERT INTO Drugs "
        "(Name, Unit, Price, StockQuantity) "
        "VALUES "
        "(:name, :unit, :price, :stock)"
    );

    query.bindValue(
        ":name",
        name.trimmed()
    );

    query.bindValue(
        ":unit",
        unit.trimmed()
    );

    query.bindValue(
        ":price",
        price
    );

    query.bindValue(
        ":stock",
        stockQuantity
    );

    if (!query.exec())
    {
        qDebug()
            << "Add Drug Error:"
            << query.lastError().text();

        return false;
    }

    return true;
}

//Edit drug info
bool Drug::UpdateDrugInfo(
    int drugID,
    const QString& name,
    const QString& unit,
    double price,
    int stockQuantity)
{
    if (drugID <= 0)
        return false;

    if (name.trimmed().isEmpty())
        return false;

    if (unit.trimmed().isEmpty())
        return false;

    if (price < 0)
        return false;

    if (stockQuantity < 0)
        return false;


    QSqlQuery query;

    query.prepare("UPDATE Drugs ""SET Name = :name, ""Unit = :unit, ""Price = :price, ""StockQuantity = :stock ""WHERE DrugID = :id");

    query.bindValue(":id", drugID);

    query.bindValue(":name", name.trimmed());

    query.bindValue(":unit", unit.trimmed());

    query.bindValue(":price", price);

    query.bindValue(":stock", stockQuantity);


    if (!query.exec())
    {
        qDebug()
            << "Update Drug Error:"
            << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool Drug::RemoveDrug(int drugID)
{
    QSqlQuery query;


    query.prepare(
        "SELECT StockQuantity "
        "FROM Drugs "
        "WHERE DrugID=:id"
    );

    query.bindValue(":id", drugID);


    query.exec();

    query.next();


    int oldStock =
        query.value(0).toInt();



    // Save history

    QSqlQuery history;


    history.prepare(
        "INSERT INTO DrugStockHistory "
        "(DrugID,Action,OldQuantity,NewQuantity,Time,Operator)"
        "VALUES "
        "(:id,'DELETE',:old,0,datetime('now'),'Receptionist')"
    );


    history.bindValue(":id", drugID);
    history.bindValue(":old", oldStock);


    history.exec();



    // Delete

    QSqlQuery del;


    del.prepare(
        "DELETE FROM Drugs "
        "WHERE DrugID=:id"
    );


    del.bindValue(":id", drugID);



    return del.exec();
}