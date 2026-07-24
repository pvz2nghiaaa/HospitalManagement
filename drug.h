#pragma once
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class Drug
{
private:
    int drugID;
    QString name;
    QString unit;
    double price;

public:
    // Constructors
    Drug();
    Drug(QString name, QString unit, double price);
    Drug(int drugID, QString name, QString unit, double price);

    // Database initialization
    static bool initTable();

    // Getters
    int getDrugID() const;
    QString getName() const;
    QString getUnit() const;
    double getPrice() const;

    // Setters
    void setDrugID(int id);
    void setName(const QString &nName);
    void setUnit(const QString &nUnit);
    void setPrice(double nPrice);

    // Fluent Setters
    Drug& SetDrugID(int id);
    Drug& SetName(const QString &nName);
    Drug& SetUnit(const QString &nUnit);
    Drug& SetPrice(double nPrice);
};
