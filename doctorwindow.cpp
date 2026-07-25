#include "doctorwindow.h"
#include "ui_DoctorWindow.h"
#include "user.h"
#include <QMessageBox>
#include "loginwindow.h"

DoctorWindow::DoctorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DoctorWindow)
{
    ui->setupUi(this);
    ui->lblAdmin->setText("Dr. " + User::GetActiveUser().GetFullName() + " (Online)");

    ui->tblMyActivity->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->dateEdit_2->setDisplayFormat("dd-MM-yyyy");
    ui->dateEdit_3->setDisplayFormat("dd-MM-yyyy");
    ui->dateEdit_2->setDate(QDate::currentDate().addDays(-30));
    ui->dateEdit_3->setDate(QDate::currentDate());

    loadProfileData();
    navigateToPage(0, ui->btnDashboard);
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
        btn->setProperty("active", false); // "active" properties along with its style is declared in .ui
        btn->style()->unpolish(btn); // refresh stylesheet
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

void DoctorWindow::on_btnSearchActivity_clicked()
{
    QString dateFrom = ui->dateEdit_2->date().toString("yyyy-MM-dd");
    QString dateTo = ui->dateEdit_3->date().toString("yyyy-MM-dd");

    QList<AttendanceLog> logs = Doctor::SearchMyActivityLogs(dateFrom, dateTo);

    ui->tblMyActivity->setRowCount(0);
    ui->tblMyActivity->setRowCount(logs.size());

    for (int i = 0; i < logs.size(); i++) {

        QString fullDateString = logs[i].getDate();
        QTableWidgetItem *itemDate = new QTableWidgetItem(fullDateString);
        QTableWidgetItem *itemTime = new QTableWidgetItem("-");
        QTableWidgetItem *itemAction = new QTableWidgetItem("Attendance");

        QString status = (logs[i].getIsPresent() == 1) ? "Present" : "Absent";
        QTableWidgetItem *itemDesc = new QTableWidgetItem(status);

        itemDate->setTextAlignment(Qt::AlignCenter);
        itemTime->setTextAlignment(Qt::AlignCenter);
        itemAction->setTextAlignment(Qt::AlignCenter);
        itemDesc->setTextAlignment(Qt::AlignCenter);

        ui->tblMyActivity->setItem(i, 0, itemDate);
        ui->tblMyActivity->setItem(i, 1, itemTime);
        ui->tblMyActivity->setItem(i, 2, itemAction);
        ui->tblMyActivity->setItem(i, 3, itemDesc);
    }
}


void DoctorWindow::loadProfileData()
{
    User& myProfile = Doctor::GetMyProfileInfo();
    ui->lineEdit_2->setText(QString::number(myProfile.GetID()));
    ui->lineEdit_3->setText(myProfile.GetFullName());
    ui->lineEdit_4->setText("N/A");
    ui->lineEdit_5->setText("N/A");
    ui->lineEdit_6->setText(myProfile.GetPhoneNumber());
    ui->lineEdit_7->setText(myProfile.GetRole());


    QList<Permission> myPerms = Doctor::GetMyPermissions();
    ui->tblMyPermissions->setRowCount(0);
    ui->tblMyPermissions->setRowCount(myPerms.size());

    QString permissionsText = "";

    for (int i = 0; i < myPerms.size(); ++i) {
        int pType = myPerms[i].toUnderlying(); 
        
        QString pName = "Unknown Permission";
        switch (pType) {
            case Permission::createRecord:
                pName = "Create Medical Record"; 
                break;
            case Permission::viewRecord:
                pName = "View Medical Record"; 
                break;
            case Permission::editRecord:
                pName = "Edit Medical Record"; 
                break;
            case Permission::manageDrugs:
                pName = "Manage Drugs";
                break;
            case Permission::viewLog:
                pName = "View Logs"; 
                break;
            case Permission::addLog:
                pName = "Add Logs";
                break;
            case Permission::changePermission:
                pName = "Change Permissions"; 
                break;
            case Permission::manageUsers:
                pName = "Manage Users"; 
                break;
            case Permission::createPatient:
                pName = "Create Patient"; 
                break;
            case Permission::editPatient:
                pName = "Edit Patient"; 
                break;
            case Permission::createInvoice:
                pName = "Create Invoice"; 
                break;
            case Permission::viewInvoice:
                pName = "View Invoice"; 
                break;
        }

        permissionsText += "- " + pName + "\n";
        QTableWidgetItem *itemPerm = new QTableWidgetItem(pName);
        QTableWidgetItem *itemStatus = new QTableWidgetItem("Granted");
        itemStatus->setTextAlignment(Qt::AlignCenter);

        ui->tblMyPermissions->setItem(i, 0, itemPerm);
        ui->tblMyPermissions->setItem(i, 1, itemStatus);
    }

}