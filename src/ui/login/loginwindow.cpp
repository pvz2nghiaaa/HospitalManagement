#include "loginwindow.h"
#include "ui_LoginWindow.h"
#include "permission.h"
#include "user.h"
#include <QMessageBox>
#include "receptionistwindow.h"
#include "adminwindow.h"
#include "doctorwindow.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    ui->usernameField->setFocus();
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_btnLogin_clicked()
{

    QString username = ui->usernameField->text();
    QString password = ui->passwordField->text();

    if (User::login(username, password)) {
        QMessageBox::information(this, "Welcome", "User: " + username + "\nWelcome to Hospital Management!");
        QString roleUser = User::GetActiveUser().GetRole();
        if (roleUser == "Admin"){
            AdminWindow *adminWin = new AdminWindow();
            adminWin->setAttribute(Qt::WA_DeleteOnClose);
            adminWin->show();
        }
        else if (roleUser == "Receptionist"){
            ReceptionistWindow *receptionistWin = new ReceptionistWindow();
            receptionistWin->setAttribute(Qt::WA_DeleteOnClose);
            receptionistWin->show();
        }
        else if (roleUser == "Doctor"){
            DoctorWindow *doctorWin = new DoctorWindow();
            doctorWin->setAttribute(Qt::WA_DeleteOnClose);
            doctorWin->show();
        }
        this->close();
    } else if (username.isEmpty() || password.isEmpty()){
        QMessageBox::warning(this, "Empty fields", "Please enter both username and password.");
    } else {
        QMessageBox::information(this, "Error", "Invalid username or password!");
    }

}

