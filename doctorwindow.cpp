#include "doctorwindow.h"
#include "ui_DoctorWindow.h"
#include "user.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QDate>
#include "loginwindow.h"

DoctorWindow::DoctorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DoctorWindow)
{
    ui->setupUi(this);
    ui->lblAdmin->setText("Dr. " + User::GetActiveUser().GetFullName() + " (Online)");
    navigateToPage(0, ui->btnDashboard);

    ui->tblPatient->setColumnCount(6);
    ui->tblPatient->setHorizontalHeaderLabels({"ID", "Full Name", "Phone", "BirthDate", "Sex", "Address"});
    ui->tblPatient->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

DoctorWindow::~DoctorWindow()
{
    delete ui;
}

void DoctorWindow::on_btnDashboard_clicked()
{
    navigateToPage(0, ui->btnDashboard);
}

void DoctorWindow::on_btnPatients_clicked()
{
    navigateToPage(1, ui->btnPatients);
}

void DoctorWindow::on_btnDoctors_clicked()
{
    navigateToPage(2, ui->btnDoctors);
}

void DoctorWindow::navigateToPage(int pageIndex, QPushButton* activeBtn){
    ui->stackedWidget->setCurrentIndex(pageIndex);
    QList<QPushButton*> sidebarButtons = {
        ui->btnDashboard,
        ui->btnPatients,
        ui->btnDoctors
    };
    for (QPushButton* btn: sidebarButtons){
        btn->setProperty("active", false);
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }

    activeBtn->setProperty("active", true);
    activeBtn->style()->unpolish(activeBtn);
    activeBtn->style()->polish(activeBtn);
}

void DoctorWindow::on_btnLogout_clicked()
{
    User::logout();
    QMessageBox::information(this, "Program info", "Logged out successfully!");

    LoginWindow *loginWin = new LoginWindow();
    loginWin->setAttribute(Qt::WA_DeleteOnClose);
    loginWin->show();

    this->close();
}

void DoctorWindow::on_btnSearch_4_clicked() {
    QString keyword = ui->txtSearch_4->text();
    currentPatientList = Doctor::SearchPatientBy(keyword);

    ui->tblPatient->setRowCount(currentPatientList.size());

    for (int i = 0; i < currentPatientList.size(); ++i) {
        const Patient& p = currentPatientList[i];
        ui->tblPatient->setItem(i, 0, new QTableWidgetItem(QString::number(p.ID)));
        ui->tblPatient->setItem(i, 1, new QTableWidgetItem(p.FullName));
        ui->tblPatient->setItem(i, 2, new QTableWidgetItem(p.Phone));
        ui->tblPatient->setItem(i, 3, new QTableWidgetItem(p.BirthDate));
        ui->tblPatient->setItem(i, 4, new QTableWidgetItem(p.Sex));
        ui->tblPatient->setItem(i, 5, new QTableWidgetItem(p.Address));
    }
}

void DoctorWindow::on_tblPatient_cellClicked(int row, int column) {
    if (row < 0 || row >= currentPatientList.size()) return;

    const Patient& p = currentPatientList[row];

    ui->txtPatientID->setText(QString::number(p.ID));
    ui->txtFullName->setText(p.FullName);
    ui->txtAddress->setText(p.Address);

    QDate dob = QDate::fromString(p.BirthDate, "yyyy-MM-dd");
    if (dob.isValid()) {
        ui->dateDOB->setDate(dob);
    }

    if (p.Sex == "Male" || p.Sex == "Nam") {
        ui->cbGender->setCurrentIndex(0);
    } else {
        ui->cbGender->setCurrentIndex(1);
    }
}

void DoctorWindow::on_btnUpdatePatient_clicked() {
    int id = ui->txtPatientID->text().toInt();
    QString name = ui->txtFullName->text();
    QString dob = ui->dateDOB->date().toString("yyyy-MM-dd");
    QString sex = ui->cbGender->currentText();
    QString address = ui->txtAddress->text();

    if (Doctor::UpdatePatientInfo(id, name, dob, sex, address)) {
        QMessageBox::information(this, "Success", "Update successful!");
        on_btnSearch_4_clicked();
    } else {
        QMessageBox::warning(this, "Error", "Update failed! Check permissions or connection.");
    }
}
