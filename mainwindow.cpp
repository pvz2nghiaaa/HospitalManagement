#include "mainwindow.h"
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
    DB_Connection.close();
    delete ui;
}

bool MainWindow::checkConnection(){
    if (!IsDBLoaded){
        DB_Connection = QSqlDatabase::addDatabase("QSQLITE");
        DB_Connection.setDatabaseName("database.db");
        if (DB_Connection.open()){
            qDebug() << "Database Is Connected";
            return IsDBLoaded = true;
        }
        qDebug() << "Database is not connected";
        qDebug() << "Error : " << DB_Connection.lastError();
        qDebug() << "Looking for DB in:" << QCoreApplication::applicationDirPath();
        return IsDBLoaded = false;
    }
    return true;
}


void MainWindow::on_pushButton_clicked()
{
    // database first
    checkConnection();
    User::initTable();
    Permission::initTable();
    // login step
    QString username = ui->usernameField->text();
    QString password = ui->passwordField->text();
    if (User::login(username, password))
        QMessageBox::information(this, "Welcome Message", "User: " + username + "\nWelcome to Hospital Management!");
    else QMessageBox::information(this, "Error Box", "Invalid username or password!");
}

