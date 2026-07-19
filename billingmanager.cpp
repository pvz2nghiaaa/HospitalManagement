#include "BillingManager.h"
#include "Invoice.h"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

BillingManager::BillingManager() {}

Invoice* BillingManager::CreateInvoice(int recordID, int patientID) {
    Invoice* newInvoice = new Invoice(recordID, patientID);

    newInvoice->autoGenerateItems();

    if (newInvoice->save()) {
        qDebug() << "Saved invoice. ID:" << newInvoice->getInvoiceID();
        return newInvoice;
    } else {
        delete newInvoice;
        return nullptr;
    }
}

QList<int> BillingManager::SearchInvoiceByPatient(int patientID) {
    QList<int> invoiceList;
    QSqlQuery query;
    query.prepare("SELECT InvoiceID FROM Invoices WHERE PatientID = :patID ORDER BY DateIssued DESC");
    query.bindValue(":patID", patientID);

    if (query.exec()) {
        while (query.next()) {
            invoiceList.append(query.value(0).toInt());
        }
    }
    return invoiceList;
}