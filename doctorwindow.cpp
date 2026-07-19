#include "doctorwindow.h"
#include "ui_DoctorWindow.h"

DoctorWindow::DoctorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DoctorWindow)
{
    ui->setupUi(this);
}

DoctorWindow::~DoctorWindow()
{
    delete ui;
}
