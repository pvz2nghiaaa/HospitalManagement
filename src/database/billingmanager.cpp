#include "billingmanager.h"
#include "invoice.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QApplication>
#include <QWidget>

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

Invoice* BillingManager::CreateInvoiceByRecordID(int recordID, QString* errorMessage) {
    if (recordID <= 0) {
        if (errorMessage) *errorMessage = "Medical Record ID must be a positive integer.";
        return nullptr;
    }

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT PatientID FROM MedicalRecords WHERE RecordID = :recID");
    checkQuery.bindValue(":recID", recordID);

    if (!checkQuery.exec()) {
        if (errorMessage) *errorMessage = "Database query failed: " + checkQuery.lastError().text();
        return nullptr;
    }

    if (!checkQuery.next()) {
        if (errorMessage) *errorMessage = QString("Medical Record ID %1 does not exist.").arg(recordID);
        return nullptr;
    }

    int patientID = checkQuery.value(0).toInt();

    Invoice* inv = CreateInvoice(recordID, patientID);
    if (!inv && errorMessage) {
        *errorMessage = "Failed to create invoice in database.";
    }
    return inv;
}

QList<InvoiceSummary> BillingManager::SearchInvoices(const QString& keyword, const QString& status, QString* errorMessage) {
    QList<InvoiceSummary> result;

    QString queryStr = "SELECT i.InvoiceID, p.FullName FROM Invoices i "
                       "JOIN MedicalRecords m ON i.RecordID = m.RecordID "
                       "JOIN Patients p ON m.PatientID = p.ID";

    QStringList conditions;

    if (!keyword.isEmpty()) {
        bool isNumber = false;
        int idVal = keyword.toInt(&isNumber);
        if (isNumber) {
            conditions.append(QString("(i.InvoiceID = %1 OR p.FullName LIKE '%%2%')").arg(idVal).arg(keyword));
        } else {
            conditions.append(QString("p.FullName LIKE '%%1%'").arg(keyword));
        }
    }

    if (status == "Paid") {
        conditions.append("i.IsPaid = 1");
    } else if (status == "Unpaid") {
        conditions.append("i.IsPaid = 0");
    }

    if (!conditions.isEmpty()) {
        queryStr += " WHERE " + conditions.join(" AND ");
    }

    queryStr += " ORDER BY i.InvoiceID DESC";

    QSqlQuery query;
    if (!query.exec(queryStr)) {
        if (errorMessage) *errorMessage = "Failed to query invoices: " + query.lastError().text();
        return result;
    }

    while (query.next()) {
        InvoiceSummary item;
        item.invoiceID = query.value(0).toInt();
        item.patientName = query.value(1).toString();
        result.append(item);
    }

    return result;
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

InvoiceDetails BillingManager::GetInvoiceDetails(
    int invoiceID)
{
    InvoiceDetails details;

    if (invoiceID <= 0)
    {
        qDebug()
            << "GetInvoiceDetails: invalid InvoiceID";

        return details;
    }

    // Lấy thông tin chính của invoice
    QSqlQuery invoiceQuery;

    invoiceQuery.prepare(
        "SELECT "
        "i.InvoiceID, "
        "i.RecordID, "
        "i.PatientID, "
        "i.DateIssued, "
        "i.TotalAmount, "
        "i.IsPaid, "
        "p.FullName AS PatientName "
        "FROM Invoices i "
        "LEFT JOIN Patients p "
        "ON i.PatientID = p.ID "
        "WHERE i.InvoiceID = :invoiceID"
    );

    invoiceQuery.bindValue(
        ":invoiceID",
        invoiceID
    );

    if (!invoiceQuery.exec())
    {
        qDebug()
            << "Get invoice details error:"
            << invoiceQuery.lastError().text();

        return details;
    }

    if (!invoiceQuery.next())
    {
        qDebug()
            << "Invoice not found:"
            << invoiceID;

        return details;
    }

    details.invoiceID =
        invoiceQuery.value("InvoiceID").toInt();

    details.recordID =
        invoiceQuery.value("RecordID").toInt();

    details.patientID =
        invoiceQuery.value("PatientID").toInt();

    details.patientName =
        invoiceQuery.value("PatientName").toString();

    details.dateIssued =
        invoiceQuery.value("DateIssued").toString();

    details.totalAmount =
        invoiceQuery.value("TotalAmount").toDouble();

    details.isPaid =
        invoiceQuery.value("IsPaid").toBool();

    // Lấy tên bác sĩ từ MedicalRecords
    QSqlQuery doctorQuery;

    doctorQuery.prepare(
        "SELECT u.FullName "
        "FROM MedicalRecords mr "
        "LEFT JOIN User u "
        "ON mr.DoctorID = u.UserID "
        "WHERE mr.RecordID = :recordID"
    );

    doctorQuery.bindValue(
        ":recordID",
        details.recordID
    );

    if (doctorQuery.exec() &&
        doctorQuery.next())
    {
        details.doctorName =
            doctorQuery.value(0).toString();
    }
    else
    {
        details.doctorName = "Unknown";
    }

    // Lấy danh sách thuốc và dịch vụ
    QSqlQuery itemQuery;

    itemQuery.prepare(
        "SELECT "
        "ItemType, "
        "Name, "
        "Price, "
        "Quantity, "
        "Unit "
        "FROM BillableItems "
        "WHERE InvoiceID = :invoiceID "
        "ORDER BY ItemID ASC"
    );

    itemQuery.bindValue(
        ":invoiceID",
        invoiceID
    );

    if (!itemQuery.exec())
    {
        qDebug()
            << "Get invoice items error:"
            << itemQuery.lastError().text();

        return details;
    }

    while (itemQuery.next())
    {
        InvoiceItemDetails item;

        item.itemType =
            itemQuery.value("ItemType").toString();

        item.name =
            itemQuery.value("Name").toString();

        item.price =
            itemQuery.value("Price").toDouble();

        item.quantity =
            itemQuery.value("Quantity").toInt();

        item.unit =
            itemQuery.value("Unit").toString();

        item.amount =
            item.price * item.quantity;

        details.items.append(item);
    }

    details.found = true;

    return details;
}


bool BillingManager::MarkInvoiceAsPaid(
    int invoiceID,
    const QString& paymentMethod)
{
    Q_UNUSED(paymentMethod);

    if (invoiceID <= 0)
    {
        qDebug()
            << "MarkInvoiceAsPaid: invalid InvoiceID";

        return false;
    }

    // Kiểm tra invoice tồn tại và trạng thái
    QSqlQuery checkQuery;

    checkQuery.prepare(
        "SELECT IsPaid "
        "FROM Invoices "
        "WHERE InvoiceID = :invoiceID"
    );

    checkQuery.bindValue(
        ":invoiceID",
        invoiceID
    );

    if (!checkQuery.exec())
    {
        qDebug()
            << "Check invoice status error:"
            << checkQuery.lastError().text();

        return false;
    }

    if (!checkQuery.next())
    {
        qDebug()
            << "Invoice not found:"
            << invoiceID;

        return false;
    }

    bool alreadyPaid =
        checkQuery.value("IsPaid").toBool();

    // Đã thanh toán rồi thì xem như thành công
    if (alreadyPaid)
    {
        qDebug()
            << "Invoice already paid:"
            << invoiceID;

        return true;
    }

    // Chuyển Unpaid thành Paid
    QSqlQuery updateQuery;

    updateQuery.prepare(
        "UPDATE Invoices "
        "SET IsPaid = 1 "
        "WHERE InvoiceID = :invoiceID "
        "AND IsPaid = 0"
    );

    updateQuery.bindValue(
        ":invoiceID",
        invoiceID
    );

    if (!updateQuery.exec())
    {
        qDebug()
            << "Mark invoice as paid error:"
            << updateQuery.lastError().text();

        return false;
    }

    return updateQuery.numRowsAffected() > 0;
}

bool BillingManager::PrintInvoice(
    int invoiceID)
{
    InvoiceDetails details =
        GetInvoiceDetails(invoiceID);

    if (!details.found)
    {
        qDebug()
            << "PrintInvoice: invoice not found";

        return false;
    }

    QString status =
        details.isPaid
        ? "Paid"
        : "Unpaid";

    QString html;

    html +=
        "<html>"
        "<head>"
        "<style>"
        "body { font-family: Arial; color: #222; }"
        "h1 { text-align: center; }"
        "table { width: 100%; border-collapse: collapse; }"
        "th, td { border: 1px solid #888; padding: 7px; }"
        "th { background-color: #eeeeee; }"
        ".right { text-align: right; }"
        ".center { text-align: center; }"
        "</style>"
        "</head>"
        "<body>";

    html +=
        "<h1>HOSPITAL INVOICE</h1>";

    html +=
        "<p><b>Invoice ID:</b> "
        + QString::number(details.invoiceID)
        + "</p>";

    html +=
        "<p><b>Record ID:</b> "
        + QString::number(details.recordID)
        + "</p>";

    html +=
        "<p><b>Patient:</b> "
        + details.patientName.toHtmlEscaped()
        + "</p>";

    html +=
        "<p><b>Doctor:</b> "
        + details.doctorName.toHtmlEscaped()
        + "</p>";

    html +=
        "<p><b>Date issued:</b> "
        + details.dateIssued.toHtmlEscaped()
        + "</p>";

    html +=
        "<p><b>Status:</b> "
        + status
        + "</p>";

    html +=
        "<table>"
        "<tr>"
        "<th>Item</th>"
        "<th>Quantity</th>"
        "<th>Price</th>"
        "<th>Amount</th>"
        "</tr>";

    for (const InvoiceItemDetails& item :
        details.items)
    {
        html += "<tr>";

        html +=
            "<td>"
            + item.name.toHtmlEscaped()
            + "</td>";

        html +=
            "<td class='center'>"
            + QString::number(item.quantity)
            + "</td>";

        html +=
            "<td class='right'>"
            + QString::number(item.price, 'f', 2)
            + "</td>";

        html +=
            "<td class='right'>"
            + QString::number(item.amount, 'f', 2)
            + "</td>";

        html += "</tr>";
    }

    html += "</table>";

    html +=
        "<h3 class='right'>Total: "
        + QString::number(
            details.totalAmount,
            'f',
            2
        )
        + " VND</h3>";

    html +=
        "</body>"
        "</html>";

    QPrinter printer(
        QPrinter::HighResolution
    );

    QPrintDialog dialog(
        &printer,
        QApplication::activeWindow()
    );

    dialog.setWindowTitle(
        "Print Invoice"
    );

    if (dialog.exec() != QDialog::Accepted)
    {
        return false;
    }

    QTextDocument document;
    document.setHtml(html);
    document.print(&printer);

    return true;
}