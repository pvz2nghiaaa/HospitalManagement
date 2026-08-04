#ifndef INVOICE_H
#define INVOICE_H

#include <QList>
#include <QString>
#include "billableitem.h"

class Invoice {
private:
    int invoiceID;
    int recordID;
    int patientID;
    QString dateIssued;
    double totalAmount;
    bool isPaid;

    QList<BillableItem*> items;

public:
    Invoice(int recordID, int patientID);
    ~Invoice();

    void addBillableItem(BillableItem* item);
    void autoGenerateItems();
    bool save();


    static bool initTable();
    int getInvoiceID() const;
    void markAsPaid();
    static int GetTotalInvoices();
};

#endif // INVOICE_H