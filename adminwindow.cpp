#include "adminwindow.h"
#include "./ui_AdminWindow.h"
#include <QMessageBox>
#include "LoginWindow.h"
#include "user.h"

AdminWindow::AdminWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    navigateToPage(0, ui->btnDashboard);
}

AdminWindow::~AdminWindow()
{
    delete ui;
}
void AdminWindow::on_btnDashboard_clicked()
{
    navigateToPage(0, ui->btnDashboard);
}


void AdminWindow::on_btnStaffManagement_clicked()
{
    navigateToPage(1, ui->btnStaffManagement);
}


void AdminWindow::on_btnPermissionManagement_clicked()
{
    navigateToPage(2, ui->btnPermissionManagement);
}


void AdminWindow::on_btnActivityLogs_clicked()
{
    navigateToPage(3, ui->btnActivityLogs);
}


void AdminWindow::on_btnReport_clicked()
{
    navigateToPage(4, ui->btnReport);
}

void AdminWindow::navigateToPage(int pageIndex, QPushButton* activeBtn){
    ui->stackedWidget->setCurrentIndex(pageIndex);
    QList<QPushButton*> sidebarButtons = {
        ui->btnDashboard,
        ui->btnStaffManagement,
        ui->btnPermissionManagement,
        ui->btnActivityLogs,
        ui->btnReport
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

void AdminWindow::on_btnLogout_clicked()
{
    User::logout();
    QMessageBox::information(this, "Program info", "Logged out successfully!");
    
    LoginWindow *loginWin = new LoginWindow();
    loginWin->setAttribute(Qt::WA_DeleteOnClose);
    loginWin->show();
    
    this->close();
}

