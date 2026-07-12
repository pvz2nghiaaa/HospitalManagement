#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    checkConnection();
}

MainWindow::~MainWindow()
{
    DB_Connection.close();
    delete ui;
}

bool MainWindow::checkConnection(){
    DB_Connection = QSqlDatabase::addDatabase("QSQLITE");
    DB_Connection.setDatabaseName("database.db");
    if (DB_Connection.open()){
        qDebug() << "Database Is Connected";
        return true;
    }
    qDebug() << "Database is not connected";
    qDebug() << "Error : " << DB_Connection.lastError();
    qDebug() << "Looking for DB in:" << QCoreApplication::applicationDirPath();
    return false;
}

