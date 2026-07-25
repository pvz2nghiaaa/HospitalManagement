#include "receptionistwindow.h"
#include "ui_ReceptionistWindow.h"
#include "user.h"
#include <QMessageBox>
#include "loginwindow.h"
ReceptionistWindow::ReceptionistWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ReceptionistWindow)
{
    ui->setupUi(this);
    ui->lblAdmin->setText("Rect. " + User::GetActiveUser().GetFullName()+ " (Online)");
    navigateToPage(0, ui->btnDashboard);
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

    QString baseSql = "SELECT ID, FullName, Phone, BirthDate, Sex, Address FROM Patients";
    QStringList conditions;

    if (!searchID.isEmpty()) {
        conditions.append("ID = :id");
    }
    if (!searchName.isEmpty()) {
        conditions.append("FullName LIKE :name");
    }
    if (!searchPhone.isEmpty()) {
        conditions.append("Phone = :phone");
    }
    if (!conditions.isEmpty()) {
        baseSql += " WHERE " + conditions.join(" AND ");
    }

    QSqlQuery query;
    query.prepare(baseSql);

    if (!searchID.isEmpty()) {
        query.bindValue(":id", searchID);
    }

    if (!searchName.isEmpty()) {
        // Adds % wildcards around input for flexible searching
        query.bindValue(":name", "%" + searchName + "%");
    }

    if (!searchPhone.isEmpty()) {
        query.bindValue(":phone", searchPhone);
    }

    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error", "Failed to search database.");
        return;
    }

    ui->tblPatient->setRowCount(0);

    int row = 0;
    while (query.next()) {
        ui->tblPatient->insertRow(row);
        qDebug() << "Found!";

        QString id    = query.value(0).toString();
        QString name  = query.value(1).toString();
        QString phone = query.value(2).toString();
        QString dob   = query.value(3).toString();
        QString sex   = query.value(4).toString();
        QString addr  = query.value(5).toString();

        ui->tblPatient->setItem(row, 0, new QTableWidgetItem(id));
        ui->tblPatient->setItem(row, 1, new QTableWidgetItem(name));
        ui->tblPatient->setItem(row, 2, new QTableWidgetItem(phone));
        ui->tblPatient->setItem(row, 3, new QTableWidgetItem(dob));
        ui->tblPatient->setItem(row, 4, new QTableWidgetItem(sex));
        ui->tblPatient->setItem(row, 5, new QTableWidgetItem(addr));

        row++;
    }

    // Optional feedback if zero rows matched
    if (row == 0) {
        QMessageBox::information(this, "No Results", "No matching records found.");
    }
}

