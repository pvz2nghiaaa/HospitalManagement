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


Drug::Drug(int drugID, QString name,QString unit, double price,int stockQuantity)
    : drugID(drugID),name(name),unit(unit),price(price),stockQuantity(stockQuantity){}

bool Drug::initTable()
{
    static bool tableLoaded = false;

    if (tableLoaded)
        return true;

    QSqlQuery drugQuery;

    if (!drugQuery.exec(
        "CREATE TABLE IF NOT EXISTS Drugs ("
        "DrugID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "Name TEXT, "
        "Unit TEXT, "
        "Price REAL, "
        "StockQuantity INTEGER NOT NULL DEFAULT 0)"
    ))
    {
        qDebug()
            << "Failed to create Drugs table:"
            << drugQuery.lastError().text();

        return false;
    }

    QSqlQuery checkQuery;

    if (!checkQuery.exec(
        "PRAGMA table_info(DrugStockHistory)"
    ))
    {
        qDebug()
            << "Cannot check history table:"
            << checkQuery.lastError().text();

        return false;
    }

    QStringList historyColumns;

    while (checkQuery.next())
    {
        historyColumns.append(
            checkQuery.value(1).toString()
        );
    }

    bool oldHistoryStructure = false;

    if (!historyColumns.isEmpty())
    {
        QStringList correctColumns;

        correctColumns
            << "HistoryID"
            << "DrugName"
            << "Action"
            << "Amount"
            << "Time";

        if (historyColumns != correctColumns)
            oldHistoryStructure = true;
    }

    if (oldHistoryStructure)
    {
        QSqlQuery dropQuery;

        if (!dropQuery.exec(
            "DROP TABLE DrugStockHistory"
        ))
        {
            qDebug()
                << "Failed to remove old history table:"
                << dropQuery.lastError().text();

            return false;
        }

        qDebug()
            << "Old DrugStockHistory table removed";
    }

    QSqlQuery historyQuery;

    if (!historyQuery.exec(
        "CREATE TABLE IF NOT EXISTS DrugStockHistory ("
        "HistoryID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "DrugName TEXT NOT NULL, "
        "Action TEXT NOT NULL, "
        "Amount INTEGER NOT NULL DEFAULT 0, "
        "Time TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP)"
    ))
    {
        qDebug()
            << "Failed to create DrugStockHistory table:"
            << historyQuery.lastError().text();

        return false;
    }

    tableLoaded = true;

    qDebug()
        << "Drugs and DrugStockHistory tables initialized";

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
    if (!initTable())
        return false;

    QString cleanName =
        name.trimmed();

    QString cleanUnit =
        unit.trimmed();

    if (cleanName.isEmpty())
    {
        qDebug() << "Drug name is empty";
        return false;
    }

    if (cleanUnit.isEmpty())
    {
        qDebug() << "Drug unit is empty";
        return false;
    }

    if (price < 0)
    {
        qDebug() << "Invalid drug price";
        return false;
    }

    if (stockQuantity < 0)
    {
        qDebug() << "Invalid stock quantity";
        return false;
    }

    QSqlQuery query;

    query.prepare(
        "INSERT INTO Drugs "
        "(Name, Unit, Price, StockQuantity) "
        "VALUES "
        "(:name, :unit, :price, :stock)"
    );

    query.bindValue(
        ":name",
        cleanName
    );

    query.bindValue(
        ":unit",
        cleanUnit
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

    if (!AddHistory(
        cleanName,
        "ADD",
        stockQuantity))
    {
        qDebug()
            << "Drug added, but history was not saved";

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
    if (!initTable())
        return false;

    if (drugID <= 0)
    {
        qDebug() << "Invalid DrugID";
        return false;
    }

    QString cleanName =
        name.trimmed();

    QString cleanUnit =
        unit.trimmed();

    if (cleanName.isEmpty())
    {
        qDebug() << "Drug name is empty";
        return false;
    }

    if (cleanUnit.isEmpty())
    {
        qDebug() << "Drug unit is empty";
        return false;
    }

    if (price < 0)
    {
        qDebug() << "Invalid drug price";
        return false;
    }

    if (stockQuantity < 0)
    {
        qDebug() << "Invalid stock quantity";
        return false;
    }

    // ==========================================
    // Lấy số lượng cũ
    // ==========================================
    QSqlQuery oldQuery;

    oldQuery.prepare(
        "SELECT StockQuantity "
        "FROM Drugs "
        "WHERE DrugID = :id"
    );

    oldQuery.bindValue(
        ":id",
        drugID
    );

    if (!oldQuery.exec())
    {
        qDebug()
            << "Get Old Drug Error:"
            << oldQuery.lastError().text();

        return false;
    }

    if (!oldQuery.next())
    {
        qDebug()
            << "Drug not found:"
            << drugID;

        return false;
    }

    int oldStockQuantity =
        oldQuery.value(0).toInt();

    // ==========================================
    // Update thuốc
    // ==========================================
    QSqlQuery updateQuery;

    updateQuery.prepare(
        "UPDATE Drugs "
        "SET Name = :name, "
        "Unit = :unit, "
        "Price = :price, "
        "StockQuantity = :stock "
        "WHERE DrugID = :id"
    );

    updateQuery.bindValue(
        ":name",
        cleanName
    );

    updateQuery.bindValue(
        ":unit",
        cleanUnit
    );

    updateQuery.bindValue(
        ":price",
        price
    );

    updateQuery.bindValue(
        ":stock",
        stockQuantity
    );

    updateQuery.bindValue(
        ":id",
        drugID
    );

    if (!updateQuery.exec())
    {
        qDebug()
            << "Update Drug Error:"
            << updateQuery.lastError().text();

        return false;
    }

    if (updateQuery.numRowsAffected() <= 0)
    {
        qDebug()
            << "No drug was updated";

        return false;
    }

    // Số lượng thay đổi
    int amount =
        stockQuantity - oldStockQuantity;

    if (!AddHistory(
        cleanName,
        "UPDATE",
        amount))
    {
        qDebug()
            << "Drug updated, but history was not saved";
    }

    return true;
}

bool Drug::RemoveDrug(int drugID)
{
    if (!initTable())
        return false;

    if (drugID <= 0)
    {
        qDebug() << "Invalid DrugID";
        return false;
    }

    // ==========================================
    // Lấy tên và số lượng trước khi xóa
    // ==========================================
    QSqlQuery selectQuery;

    selectQuery.prepare(
        "SELECT Name, StockQuantity "
        "FROM Drugs "
        "WHERE DrugID = :id"
    );

    selectQuery.bindValue(
        ":id",
        drugID
    );

    if (!selectQuery.exec())
    {
        qDebug()
            << "Find Drug Error:"
            << selectQuery.lastError().text();

        return false;
    }

    if (!selectQuery.next())
    {
        qDebug()
            << "Drug not found:"
            << drugID;

        return false;
    }

    QString drugName =
        selectQuery.value(0).toString();

    int oldStockQuantity =
        selectQuery.value(1).toInt();

    // ==========================================
    // Xóa thuốc
    // ==========================================
    QSqlQuery deleteQuery;

    deleteQuery.prepare(
        "DELETE FROM Drugs "
        "WHERE DrugID = :id"
    );

    deleteQuery.bindValue(
        ":id",
        drugID
    );

    if (!deleteQuery.exec())
    {
        qDebug()
            << "Delete Drug Error:"
            << deleteQuery.lastError().text();

        return false;
    }

    if (deleteQuery.numRowsAffected() <= 0)
    {
        qDebug()
            << "No drug was deleted";

        return false;
    }

    // Khi xóa, toàn bộ stock bị giảm nên dùng số âm
    if (!AddHistory(
        drugName,
        "DELETE",
        -oldStockQuantity))
    {
        qDebug()
            << "Drug deleted, but history was not saved";
    }

    return true;
}
bool Drug::AddHistory(
    const QString& drugName,
    const QString& action,
    int amount)
{
    if (!initTable())
    {
        qDebug()
            << "AddHistory: cannot initialize tables";

        return false;
    }

    QSqlQuery query;

    query.prepare(
        "INSERT INTO DrugStockHistory "
        "(DrugName, Action, Amount, Time) "
        "VALUES "
        "(:name, :action, :amount, "
        "datetime('now', 'localtime'))"
    );

    query.bindValue(
        ":name",
        drugName.trimmed()
    );

    query.bindValue(
        ":action",
        action.trimmed()
    );

    query.bindValue(
        ":amount",
        amount
    );

    if (!query.exec())
    {
        qDebug()
            << "Add History Error:"
            << query.lastError().text();

        return false;
    }

    qDebug()
        << "History saved:"
        << drugName
        << action
        << amount;

    return true;
}