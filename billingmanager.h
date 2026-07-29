#ifndef BILLINGMANAGER_H
#define BILLINGMANAGER_H

#include <QList>
#include "invoice.h"

class BillingManager {
public:
    BillingManager();

    Invoice* CreateInvoice(int recordID, int patientID);
    QList<int> SearchInvoiceByPatient(int patientID);
};

#endif // BILLINGMANAGER_H
