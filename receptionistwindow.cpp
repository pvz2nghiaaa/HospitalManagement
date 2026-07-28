#include "receptionistwindow.h"
#include "ui_ReceptionistWindow.h"
#include "user.h"
#include "drug.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QDebug>
#include "loginwindow.h"
#include "patient.h"
ReceptionistWindow::ReceptionistWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ReceptionistWindow)
{
    ui->setupUi(this);
    ui->lblAdmin->setText("Rect. " + User::GetActiveUser().GetFullName()+ " (Online)");
    navigateToPage(0, ui->btnDashboard);


    
        // Drug Management
    ui->tblPatient_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblPatient_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblPatient_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblPatient_2->horizontalHeader()->setStretchLastSection(true);
    // Load all drugs
    loadAllDrugs();
}

ReceptionistWindow::~ReceptionistWindow()
{
    delete ui;
}

void ReceptionistWindow::on_btnDashboard_clicked()
{
    navigateToPage(0, ui->btnDashboard);
}


void ReceptionistWindow::on_btnPatients_clicked()
{
    navigateToPage(1, ui->btnPatients);
}


void ReceptionistWindow::on_btnPatients_2_clicked()
{
    navigateToPage(2, ui->btnPatients_2);
}


void ReceptionistWindow::on_btnPatients_3_clicked()
{
    navigateToPage(3, ui->btnPatients_3);
}


void ReceptionistWindow::on_btnDoctors_clicked()
{
    navigateToPage(4, ui->btnDoctors);
}

void ReceptionistWindow::navigateToPage(int pageIndex, QPushButton* activeBtn){
    ui->stackedWidget->setCurrentIndex(pageIndex);
    QList<QPushButton*> sidebarButtons = {
        ui->btnDashboard,
        ui->btnPatients,
        ui->btnPatients_2,
        ui->btnPatients_3,
        ui->btnDoctors
    };
    for (QPushButton* btn: sidebarButtons){
        btn->setProperty("active", false); // "active" properties along with its style is declared in .ui
        btn->style()->unpolish(btn); // refresh stylesheet
        btn->style()->polish(btn);
    }

    activeBtn->setProperty("active", true);
    activeBtn->style()->unpolish(activeBtn);
    activeBtn->style()->polish(activeBtn);

}


void ReceptionistWindow::on_btnLogout_clicked()
{
    User::logout();
    QMessageBox::information(this, "Program info", "Logged out successfully!");

    LoginWindow *loginWin = new LoginWindow();
    loginWin->setAttribute(Qt::WA_DeleteOnClose);
    loginWin->show();

    this->close();
}


void ReceptionistWindow::on_btnPatientSearch_clicked()
{
    QString searchID = ui->patientIDLineEdit->text().trimmed();
    QString searchName = ui->patientNameLineEdit->text().trimmed();
    QString searchPhone = ui->patientPhoneLineEdit->text().trimmed();

    QList<std::tuple<QString, QString, QString, QString, QString, QString>> lstPatient = Patient::findPatient(searchID, searchName, searchPhone);

    ui->tblPatient->setRowCount(0);

    int row = 0;
    for (; row < (int)lstPatient.size(); row++) {
        ui->tblPatient->insertRow(row);

        ui->tblPatient->setItem(row, 0, new QTableWidgetItem(get<0>(lstPatient[row])));
        ui->tblPatient->setItem(row, 1, new QTableWidgetItem(get<1>(lstPatient[row])));
        ui->tblPatient->setItem(row, 2, new QTableWidgetItem(get<2>(lstPatient[row])));
        ui->tblPatient->setItem(row, 3, new QTableWidgetItem(get<3>(lstPatient[row])));
        ui->tblPatient->setItem(row, 4, new QTableWidgetItem(get<4>(lstPatient[row])));
        ui->tblPatient->setItem(row, 5, new QTableWidgetItem(get<5>(lstPatient[row])));
    }

    // Optional feedback if zero rows matched
    if (row == 0) {
        QMessageBox::information(this, "No Results", "No matching records found.");
    }
}

 // Drug Management
QString ReceptionistWindow::getDrugStatus(int stockQuantity) const
{
    if (stockQuantity <= 0)
        return "Out of Stock";

    if (stockQuantity <= 10)
        return "Low Stock";

    return "Available";
}
//Search
void ReceptionistWindow::SearchDrugsBy(const QString &nameOrID,const QString &stockStatus)
{
    QSqlQuery query;

    QString sql =
        "SELECT DrugID, Name AS DrugName, Unit, Price, StockQuantity "
        "FROM Drugs "
        "WHERE 1 = 1 ";

    QString keyword = nameOrID.trimmed();

    // Search theo DrugID hoặc Name
    if (!keyword.isEmpty())
    {
        sql +=
            "AND (CAST(DrugID AS TEXT) LIKE :keyword "
            "OR Name LIKE :keyword) ";
    }

    // Filter theo Stock Status
    if (stockStatus == "Available")
    {
        sql += "AND StockQuantity > 10 ";
    }
    else if (stockStatus == "Low Stock")
    {
        sql +=
            "AND StockQuantity > 0 "
            "AND StockQuantity <= 10 ";
    }
    else if (stockStatus == "Out of Stock")
    {
        sql += "AND StockQuantity = 0 ";
    }

    sql += "ORDER BY DrugID ASC";

    query.prepare(sql);

    if (!keyword.isEmpty())
    {
        query.bindValue(
            ":keyword",
            "%" + keyword + "%"
        );
    }

    if (!query.exec())
    {
        QMessageBox::critical(
            this,
            "Database Error",
            query.lastError().text()
        );

        return;
    }

    // Xóa dữ liệu cũ
    ui->tblPatient_2->setRowCount(0);

    int row = 0;

    while (query.next())
    {
        ui->tblPatient_2->insertRow(row);

        // Column 0: Drug ID
        ui->tblPatient_2->setItem(row,0,new QTableWidgetItem(query.value("DrugID").toString()));

        // Column 1: Drug Name
        ui->tblPatient_2->setItem(row,1,new QTableWidgetItem(query.value("DrugName").toString())
        );

        // Column 2: Unit
        ui->tblPatient_2->setItem(row,2,new QTableWidgetItem(query.value("Unit").toString()));

        // Column 3: Price
        ui->tblPatient_2->setItem(row,3,new QTableWidgetItem(QString::number(query.value("Price").toDouble(),'f',2)));

        int stock =query.value("StockQuantity").toInt();

        // Column 4: Stock
        ui->tblPatient_2->setItem(row,4,new QTableWidgetItem(QString::number(stock)));

        // Column 5: Status
        ui->tblPatient_2->setItem(row,5,new QTableWidgetItem(getDrugStatus(stock)));
        row++;
    }
}

void ReceptionistWindow::on_btnSearch_14_clicked()
{
    QString keyword =
        ui->txtSearch_8->text();

    QString stockStatus =
        ui->comboBox_3->currentText();

    SearchDrugsBy(keyword,stockStatus);
}


//Refresh
void ReceptionistWindow::loadAllDrugs()
{
    SearchDrugsBy("","All");
}

void ReceptionistWindow::on_btnSearch_18_clicked()
{
    ui->txtSearch_8->clear();
    ui->comboBox_3->setCurrentIndex(0);
    loadAllDrugs();
}



//Update/Add druvg
bool ReceptionistWindow::AddNewDrug(
    const QString &name,
    const QString &unit,
    double price,
    int stockQuantity)
{
    if (name.trimmed().isEmpty())
        return false;

    if (unit.trimmed().isEmpty())
        return false;

    if (price < 0)
        return false;

    if (stockQuantity < 0)
        return false;

    QSqlQuery query;

    query.prepare(
        "INSERT INTO Drugs "
        "(Name, Unit, Price, StockQuantity) "
        "VALUES "
        "(:name, :unit, :price, :stock)"
    );

    query.bindValue(
        ":name",
        name.trimmed()
    );

    query.bindValue(
        ":unit",
        unit.trimmed()
    );

    query.bindValue(
        ":price",
        price
    );

    query.bindValue(
        ":stock",
        stockQuantity
    );

    if (!query.exec())
    {
        qDebug()
            << "Add Drug Error:"
            << query.lastError().text();

        return false;
    }

    return true;
}

void ReceptionistWindow::on_btnSearch_15_clicked()
{
    bool ok;
    
    // Drug Name
    QString name =QInputDialog::getText(this,"Add New Drug","Drug Name:",QLineEdit::Normal,"",&ok);
    if (!ok)
        return;
    name = name.trimmed();
    if (name.isEmpty())
    {
        QMessageBox::warning(this,"Invalid Input","Drug name cannot be empty."
        );
        return;
    }

    // Unit
    QString unit =
        QInputDialog::getText(this,"Add New Drug","Unit:",QLineEdit::Normal,"",&ok);
    if (!ok)
        return;
    unit = unit.trimmed();

    // Price
    double price =QInputDialog::getDouble(this,"Add New Drug","Price:",0.0,0.0,1000000000.0,2,&ok);
    if (!ok)
        return;

    // Stock Quantity
    int stock =QInputDialog::getInt(this,"Add New Drug","Stock Quantity:",0,0,1000000,1,&ok);
    if (!ok)
        return;


    // Add to database=
    if (AddNewDrug(name,unit,price,stock)){
        QMessageBox::information(this,"Success","New drug added successfully.");

        loadAllDrugs();
    }
    else
    {
        QMessageBox::critical(this,"Error","Failed to add new drug."
        );
    }
}


//Edit drug info
bool ReceptionistWindow::UpdateDrugInfo(
    int drugID,
    const QString &name,
    const QString &unit,
    double price,
    int stockQuantity)
{
    if (drugID <= 0)
        return false;

    if (name.trimmed().isEmpty())
        return false;

    if (unit.trimmed().isEmpty())
        return false;

    if (price < 0)
        return false;

    if (stockQuantity < 0)
        return false;


    QSqlQuery query;

    query.prepare("UPDATE Drugs ""SET Name = :name, ""Unit = :unit, ""Price = :price, ""StockQuantity = :stock ""WHERE DrugID = :id");

    query.bindValue(":id",drugID );

    query.bindValue(":name",name.trimmed());

    query.bindValue(":unit",unit.trimmed());

    query.bindValue(":price",price);

    query.bindValue(":stock",stockQuantity);


    if (!query.exec())
    {
        qDebug()
            << "Update Drug Error:"
            << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

void ReceptionistWindow::on_btnSearch_16_clicked()
{
    int row =ui->tblPatient_2->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(this,"Warning","Please select a drug first."
        );
        return;
    }


    // Get current drug
    int drugID =
        ui->tblPatient_2
            ->item(row, 0)
            ->text()
            .toInt();

    QString oldName =
        ui->tblPatient_2
            ->item(row, 1)
            ->text();

    QString oldUnit =
        ui->tblPatient_2
            ->item(row, 2)
            ->text();

    double oldPrice =
        ui->tblPatient_2
            ->item(row, 3)
            ->text()
            .toDouble();

    int oldStock =
        ui->tblPatient_2
            ->item(row, 4)
            ->text()
            .toInt();


    bool ok;


    // New Drug Name
    QString name =QInputDialog::getText(this,"Update Drug","Drug Name:",QLineEdit::Normal,oldName,&ok);
    if (!ok)
        return;


    // New Unit
    QString unit =QInputDialog::getText(this,"Update Drug","Unit:",QLineEdit::Normal,oldUnit,&ok);
    if (!ok)
        return;


    // New Price
    double price =QInputDialog::getDouble(this,"Update Drug","Price:",oldPrice,0.0,1000000000.0,2,&ok);
    if (!ok)
        return;


    // New Stock
    int stock =QInputDialog::getInt(this,"Update Drug","Stock Quantity:",oldStock,0,1000000,1,&ok);
    if (!ok)
        return;


    // Update database
    if (UpdateDrugInfo(drugID,name,unit,price,stock))
    {
        QMessageBox::information(this,"Success","Drug information updated successfully.");

        loadAllDrugs();
    }
    else
    {
        QMessageBox::critical(this,"Error","Failed to update drug."
        );
    }
}

void ReceptionistWindow::on_tblPatient_2_cellClicked(int row,int column){
    Q_UNUSED(column);

    if (row < 0)
        return;

    int drugID =
        ui->tblPatient_2
            ->item(row, 0)
            ->text()
            .toInt();

    QString drugName =
        ui->tblPatient_2
            ->item(row, 1)
            ->text();

    qDebug()
        << "Selected Drug:"
        << drugID
        << drugName;
}


bool ReceptionistWindow::RemoveDrug(int drugID)
{
    QSqlQuery query;


    query.prepare(
        "SELECT StockQuantity "
        "FROM Drugs "
        "WHERE DrugID=:id"
    );

    query.bindValue(":id", drugID);


    query.exec();

    query.next();


    int oldStock =
        query.value(0).toInt();



    // Save history

    QSqlQuery history;


    history.prepare(
        "INSERT INTO DrugStockHistory "
        "(DrugID,Action,OldQuantity,NewQuantity,Time,Operator)"
        "VALUES "
        "(:id,'DELETE',:old,0,datetime('now'),'Receptionist')"
    );


    history.bindValue(":id", drugID);
    history.bindValue(":old", oldStock);


    history.exec();



    // Delete

    QSqlQuery del;


    del.prepare(
        "DELETE FROM Drugs "
        "WHERE DrugID=:id"
    );


    del.bindValue(":id", drugID);



    return del.exec();
}
void ReceptionistWindow::on_btnSearch_17_clicked()
{

    int row =
        ui->tblPatient_2->currentRow();


    if (row < 0)
    {
        QMessageBox::warning(
            this,
            "Delete",
            "Select a drug first"
        );
        return;
    }



    int drugID =
        ui->tblPatient_2
        ->item(row, 0)
        ->text()
        .toInt();



    if (RemoveDrug(drugID))
    {

        QMessageBox::information(
            this,
            "Delete",
            "Delete successfully"
        );


        loadAllDrugs();

    }

}

void ReceptionistWindow::GetDrugStockHistory(int drugID)
{

    QSqlQuery query;


    query.prepare(
        "SELECT Action,"
        "OldQuantity,"
        "NewQuantity,"
        "Time,"
        "Operator "
        "FROM DrugStockHistory "
        "WHERE DrugID=:id"
    );


    query.bindValue(":id", drugID);



    query.exec();



    QString result;



    while (query.next())
    {
        result +=
            query.value(0).toString()
            + " | "
            + query.value(1).toString()
            + " -> "
            + query.value(2).toString()
            + "\n";
    }



    QMessageBox::information(
        this,
        "Drug History",
        result
    );
}

void ReceptionistWindow::on_pushButton_clicked()
{

    int row =
        ui->tblPatient_2->currentRow();



    if (row < 0)
    {
        QMessageBox::warning(
            this,
            "History",
            "Select drug first"
        );

        return;
    }



    int drugID =
        ui->tblPatient_2
        ->item(row, 0)
        ->text()
        .toInt();



    GetDrugStockHistory(drugID);
}
