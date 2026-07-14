#include "mainwindow.h"
#include "adminwindow.h"
#include "./ui_mainwindow.h"
#include "permission.h"
#include "user.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
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