#include "adminwindow.h"
#include "./ui_AdminWindow.h"
#include <QMessageBox>
#include "LoginWindow.h"
#include "user.h"
#include <QTimer>
#include "patient.h"
#include "medicalrecord.h"
#include "invoice.h"
#include <QGraphicsBlurEffect>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "admin.h"

AdminWindow::AdminWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    ui->overlayFrame->hide(); // Hide overlay by default
    navigateToPage(0, ui->btnDashboard);
    updateDashboardInfo();

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AdminWindow::updateDashboardInfo);
    timer->start(7000); // cứ cách 7 giây thì dashboard update 1 lần
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

void AdminWindow::updateDashboardInfo() {
    ui->totalStaff->setText(QString::number(User::GetTotalStaff()));
    ui->totalPatients->setText(QString::number(Patient::GetTotalPatients()));
    ui->totalRecords->setText(QString::number(MedicalRecord::GetTotalRecord()));
    ui->totalInvoices->setText(QString::number(Invoice::GetTotalInvoices()));
}

void AdminWindow::on_btnAddStaff_clicked() {
    showOverlayForm();
}

void AdminWindow::on_btnAddStaffQuick_clicked() {
    showOverlayForm();
}

void AdminWindow::on_btnCancelNewStaff_clicked() {
    hideOverlayForm();
}

void AdminWindow::on_btnSaveNewStaff_clicked() {
    QString username = ui->txtNewUsername->text().trimmed();
    QString password = ui->txtNewPassword->text();
    QString fullName = ui->txtNewFullName->text().trimmed();
    QString phone = ui->txtNewPhone->text().trimmed();
    
    if (username.isEmpty() || password.isEmpty() || fullName.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Username, Password, and Full Name are required.");
        return;
    }

    RoleTemplate role = RoleTemplate::DoctorTemplate;
    if (ui->cbNewRole->currentText() == "Receptionist") {
        role = RoleTemplate::ReceptionistTemplate;
    } else if (ui->cbNewRole->currentText() == "Admin") {
        role = RoleTemplate::AdminTemplate;
    }

    // Call backend
    bool success = Admin::createNewAccount(username, password, fullName, phone, role);
    if (success) {
        QMessageBox::information(this, "Success", "Staff account created successfully!");
        updateDashboardInfo();
        
        // Reset fields
        ui->txtNewUsername->clear();
        ui->txtNewPassword->clear();
        ui->txtNewFullName->clear();
        ui->txtNewPhone->clear();
        ui->cbNewRole->setCurrentIndex(0);
        
        hideOverlayForm();
    } else {
        QMessageBox::critical(this, "Error", "Failed to create staff account. The username might already be in use.");
    }
}

void AdminWindow::showOverlayForm() {
    // 1. Create separate blur objects for each background element (no stealing)
    QGraphicsBlurEffect *blurSidebar = new QGraphicsBlurEffect(this);
    blurSidebar->setBlurRadius(8);
    ui->layoutWidget->setGraphicsEffect(blurSidebar);

    QGraphicsBlurEffect *blurBrand = new QGraphicsBlurEffect(this);
    blurBrand->setBlurRadius(8);
    ui->lblBrand->setGraphicsEffect(blurBrand);

    QGraphicsBlurEffect *blurWelcome = new QGraphicsBlurEffect(this);
    blurWelcome->setBlurRadius(8);
    ui->lblWelcome->setGraphicsEffect(blurWelcome);

    QGraphicsBlurEffect *blurLogout = new QGraphicsBlurEffect(this);
    blurLogout->setBlurRadius(8);
    ui->btnLogout->setGraphicsEffect(blurLogout);

    QGraphicsBlurEffect *blurContent = new QGraphicsBlurEffect(this);
    blurContent->setBlurRadius(8);
    ui->stackedWidget->setGraphicsEffect(blurContent);

    QGraphicsBlurEffect *blurTitle = new QGraphicsBlurEffect(this);
    blurTitle->setBlurRadius(8);
    ui->lblTitle->setGraphicsEffect(blurTitle);

    QGraphicsBlurEffect *blurAdmin = new QGraphicsBlurEffect(this);
    blurAdmin->setBlurRadius(8);
    ui->lblAdmin->setGraphicsEffect(blurAdmin);

    // 2. Clear any active graphics effect on the overlay so it stays sharp
    ui->overlayFrame->setGraphicsEffect(nullptr);

    // 3. Show overlay frame (raised to top)
    ui->overlayFrame->show();
    ui->overlayFrame->raise();

    // 4. Setup opacity animation for smooth fade-in
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(ui->overlayFrame);
    ui->overlayFrame->setGraphicsEffect(opacityEffect);

    QPropertyAnimation *fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    fadeAnimation->setDuration(300);
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void AdminWindow::hideOverlayForm() {
    // 1. Remove all background blurs
    ui->layoutWidget->setGraphicsEffect(nullptr);
    ui->lblBrand->setGraphicsEffect(nullptr);
    ui->lblWelcome->setGraphicsEffect(nullptr);
    ui->btnLogout->setGraphicsEffect(nullptr);
    ui->stackedWidget->setGraphicsEffect(nullptr);
    ui->lblTitle->setGraphicsEffect(nullptr);
    ui->lblAdmin->setGraphicsEffect(nullptr);

    // 2. Hide overlay
    ui->overlayFrame->hide();
}

