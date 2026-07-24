#ifndef LABTEST_H
#define LABTEST_H

#include "billableitem.h"

class LabTest : public BillableItem {
public:
    LabTest(int id, const QString& name, double price, int invId)
        : BillableItem(id, "LAB", name, price, 1, "Lần", invId) {}

    double calculateCost() const override {
        return price;
    }
};

#endif // LABTEST_H