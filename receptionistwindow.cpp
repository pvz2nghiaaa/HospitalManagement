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

