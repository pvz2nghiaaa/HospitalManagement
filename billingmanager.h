#ifndef BILLINGMANAGER_H
#define BILLINGMANAGER_H

#include <QString>
#include <QList>

class Invoice;

// Một dòng thuốc hoặc dịch vụ trong hóa đơn
struct InvoiceItemDetails
{
    QString itemType;
    QString name;
    double price;
    int quantity;
    QString unit;
    double amount;
};

// Toàn bộ dữ liệu chi tiết hóa đơn
struct InvoiceDetails
{
    bool found = false;

    int invoiceID = -1;
    int recordID = -1;
    int patientID = -1;

    QString patientName;
    QString doctorName;
    QString dateIssued;

    double totalAmount = 0.0;
    bool isPaid = false;

    QList<InvoiceItemDetails> items;
};

class BillingManager
{
public:
    BillingManager();

    static Invoice* CreateInvoice(
        int recordID,
        int patientID
    );

    static QList<int> SearchInvoiceByPatient(
        int patientID
    );



    static InvoiceDetails GetInvoiceDetails(
        int invoiceID
    );

    static bool MarkInvoiceAsPaid(
        int invoiceID,
        const QString& paymentMethod
    );

    static bool PrintInvoice(
        int invoiceID
    );
};


#endif // BILLINGMANAGER_H
