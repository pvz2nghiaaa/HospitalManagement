#ifndef DRUGITEM_H
#define DRUGITEM_H

#include "billableitem.h"

class DrugItem : public BillableItem {
public:
    DrugItem(int id, const QString& name, double price, int qty, const QString& unit, int invId)
        : BillableItem(id, "DRUG", name, price, qty, unit, invId) {}

    double calculateCost() const override {
        return price * quantity;
    }
};

#endif // DRUGITEM_H