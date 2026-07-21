#include "doctorwindow.h"
#include "ui_DoctorWindow.h"
#include "user.h"

DoctorWindow::DoctorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DoctorWindow)
{
    ui->setupUi(this);
    ui->lblAdmin->setText("Dr. " + User::GetActiveUser().GetFullName());
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

