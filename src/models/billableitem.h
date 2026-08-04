#ifndef BILLABLE_ITEMS_H
#define BILLABLE_ITEMS_H

#include <QString>

class BillableItem {
protected:
    int itemID;
    QString itemType;
    QString name;
    double price;
    int quantity;
    QString unit;
    int invoiceID;

public:
    BillableItem(QString name, double price, int qty, QString unit, QString type);
    virtual ~BillableItem();

    void setInvoiceID(int id);

    static bool initTable();
    virtual double CalculateCost() const = 0;
    bool save();
};

class DrugItem : public BillableItem {
public:
    DrugItem(QString name, double price, int qty, QString unit);
    double CalculateCost() const override;
    static bool initTable();
};

class LabTest : public BillableItem {
public:
    LabTest(QString name, double price);
    double CalculateCost() const override;
    static bool initTable();
};

#endif // BILLABLE_ITEMS_H