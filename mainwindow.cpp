#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "permission.h"
#include "user.h"

bool User::UserLoaded = false;

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
    checkConnection();
    User::initTable();
    Permission::initTable();
}

