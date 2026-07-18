#include "loginwindow.h"
#include "ui_LoginWindow.h"
#include "permission.h"
#include "user.h"
#include <QMessageBox>
#include "adminwindow.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
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

        AdminWindow *adminWin = new AdminWindow();
        adminWin->setAttribute(Qt::WA_DeleteOnClose);
        adminWin->show();

        this->close();
    } else {
        QMessageBox::information(this, "Error", "Invalid username or password!");
    }

}

