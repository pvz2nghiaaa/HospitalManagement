#pragma once
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QList>
class Drug
{
private:
    int drugID;
    QString name;
    QString unit;
    double price;
    int stockQuantity;

public:
    // Constructors
    Drug();
    Drug(QString name, QString unit, double price);
    Drug(QString name, QString unit, double price, int stockQuantity);
    Drug(int drugID, QString name, QString unit, double price);
    Drug(int drugID, QString name, QString unit, double price, int stockQuantity);


    // Database initialization
    static bool initTable();

    // Getters
    int getDrugID() const;
    QString getName() const;
    QString getUnit() const;
    double getPrice() const;
    int getStockQuantity() const;

    // Setters
    void setDrugID(int id);
    void setName(const QString& nName);
    void setUnit(const QString& nUnit);
    void setPrice(double nPrice);
    void setStockQuantity(int quantity);

    // Fluent Setters
    Drug& SetDrugID(int id);
    Drug& SetName(const QString& nName);
    Drug& SetUnit(const QString& nUnit);
    Drug& SetPrice(double nPrice);
    Drug& SetStockQuantity(int quantity);

    //Drug func
    static QList<Drug> SearchDrugsBy(const QString& nameOrID, const QString& stockStatus);

    static QString GetDrugStatus(int stockQuantity);

    static bool AddNewDrug(
        const QString& name,
        const QString& unit,
        double price,
        int stockQuantity);

    static bool UpdateDrugInfo(
        int drugID,
        const QString& name,
        const QString& unit,
        double price,
        int stockQuantity);

    static bool RemoveDrug(int drugID);

};
