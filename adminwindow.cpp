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
    ui->overlayPatientFrame->hide(); // Hide patient overlay by default
    ui->overlayEditStaffFrame->hide(); // Hide edit staff overlay by default
    ui->overlayDeleteStaffFrame->hide(); // Hide delete staff overlay by default
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
    refreshStaffDashboard(User::GetAllUser());
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

void AdminWindow::refreshStaffDashboard(vector<tuple<int, QString, QString, QString, bool, QString>> listUser){
    int n = listUser.size();
    ui->tblStaff->setRowCount(0); // clear old list
    for (int i = 0; i < n; i++){
        tuple<int, QString, QString, QString, bool, QString>& user = listUser[i];
        ui->tblStaff->insertRow(i);
        // row - column - data
        ui->tblStaff->setItem(i, 0, new QTableWidgetItem(QString::number(get<0>(user))));
        ui->tblStaff->setItem(i, 1, new QTableWidgetItem(get<1>(user)));
        ui->tblStaff->setItem(i, 2, new QTableWidgetItem(get<2>(user)));
        ui->tblStaff->setItem(i, 3, new QTableWidgetItem(get<3>(user)));
        ui->tblStaff->setItem(i, 5, new QTableWidgetItem(get<5>(user)));
        QTableWidgetItem* statusItem = new QTableWidgetItem(get<4>(user) ? "Active" : "Inactive");
        if (get<4>(user)) {
            statusItem->setForeground(QBrush(QColor(34, 197, 94))); // Green text for Active
        } else {
            statusItem->setForeground(QBrush(QColor(239, 68, 68))); // Red text for Inactive
        }
        ui->tblStaff->setItem(i, 4, statusItem);
    }
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

    QString role = ui->cbNewRole->currentText();

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

        refreshStaffDashboard(User::GetAllUser());
    } else {
        QMessageBox::critical(this, "Error", "Failed to create staff account. The username might already be in use.");
    }
}

void AdminWindow::showOverlayForm() {
    // 1. Disable all background interactions to prevent hover paint updates and clicks
    ui->bgWidget->setEnabled(false);

    // 2. Set geometry to cover the full window size dynamically
    ui->overlayFrame->setGeometry(0, 0, this->width(), this->height());
    int cardX = (this->width() - ui->cardCreateStaff->width()) / 2;
    int cardY = (this->height() - ui->cardCreateStaff->height()) / 2;
    ui->cardCreateStaff->move(cardX, cardY);

    // 3. Apply single blur to background container widget
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(this);
    blur->setBlurRadius(8);
    ui->bgWidget->setGraphicsEffect(blur);
    ui->bgWidget->repaint();

    // 4. Clear any active graphics effect on the overlay so it stays sharp
    ui->overlayFrame->setGraphicsEffect(nullptr);

    // 5. Show overlay frame (raised to top)
    ui->overlayFrame->show();
    ui->overlayFrame->raise();

    // 6. Setup opacity animation for smooth fade-in
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(ui->overlayFrame);
    ui->overlayFrame->setGraphicsEffect(opacityEffect);

    QPropertyAnimation *fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    fadeAnimation->setDuration(100);
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void AdminWindow::hideOverlayForm() {
    // 1. Remove background blur and re-enable background interactions
    ui->bgWidget->setGraphicsEffect(nullptr);
    ui->bgWidget->setEnabled(true);

    // 2. Hide overlay
    ui->overlayFrame->hide();
}

void AdminWindow::on_btnRegisterPatientQuick_clicked() {
    showPatientOverlay();
}

void AdminWindow::on_btnCancelPat_clicked() {
    hidePatientOverlay();
}

void AdminWindow::on_btnSavePat_clicked() {
    QString fullName = ui->txtPatFullName->text().trimmed();
    QString phoneNo = ui->txtPatPhone->text().trimmed();
    QString birthDate = ui->datePatDOB->date().toString("yyyy-MM-dd");
    QString sex = ui->cbPatGender->currentText();
    QString address = ui->txtPatAddress->text().trimmed();

    if (fullName.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Patient full name is required.");
        return;
    }

    bool success = Patient::createPatient(fullName, phoneNo, birthDate, sex, address);
    if (success) {
        QMessageBox::information(this, "Success", "Patient registered successfully!");
        updateDashboardInfo();
        
        // Reset fields
        ui->txtPatFullName->clear();
        ui->txtPatPhone->clear();
        ui->datePatDOB->setDate(QDate::currentDate());
        ui->cbPatGender->setCurrentIndex(0);
        ui->txtPatAddress->clear();
        
        hidePatientOverlay();
    } else {
        QMessageBox::critical(this, "Error", "Failed to register patient in database.");
    }
}

void AdminWindow::showPatientOverlay() {
    // 1. Disable all background interactions to prevent hover paint updates and clicks
    ui->bgWidget->setEnabled(false);

    // 2. Set geometry to cover the full window size dynamically
    ui->overlayPatientFrame->setGeometry(0, 0, this->width(), this->height());
    int cardX = (this->width() - ui->cardRegisterPatient->width()) / 2;
    int cardY = (this->height() - ui->cardRegisterPatient->height()) / 2;
    ui->cardRegisterPatient->move(cardX, cardY);

    // 3. Apply single blur to background container widget
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(this);
    blur->setBlurRadius(8);
    ui->bgWidget->setGraphicsEffect(blur);
    ui->bgWidget->repaint();

    // 4. Clear any active graphics effect on the patient overlay
    ui->overlayPatientFrame->setGraphicsEffect(nullptr);

    // 5. Show overlay frame (raised to top)
    ui->overlayPatientFrame->show();
    ui->overlayPatientFrame->raise();

    // 6. Setup opacity animation for smooth fade-in
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(ui->overlayPatientFrame);
    ui->overlayPatientFrame->setGraphicsEffect(opacityEffect);

    QPropertyAnimation *fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    fadeAnimation->setDuration(100);
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void AdminWindow::hidePatientOverlay() {
    // 1. Remove background blur and re-enable background interactions
    ui->bgWidget->setGraphicsEffect(nullptr);
    ui->bgWidget->setEnabled(true);

    // 2. Hide overlay
    ui->overlayPatientFrame->hide();
}


void AdminWindow::on_btnRefreshStaff_clicked()
{
    refreshStaffDashboard(User::GetAllUser());
}


void AdminWindow::on_btnSearch_clicked()
{
    vector<tuple<int, QString, QString, QString, bool, QString>> listUser;
    QString name = ui->txtSearch->text();
    QString trimmedName = name.trimmed();
    QString role = ui->cbRole->currentText();
    if (role == "All"){
        if (trimmedName.size() == 0){
            listUser = User::GetAllUser();
        }
        else listUser = User::SearchUserBy(ui->txtSearch->text()); // remove trim
    }
    else if (trimmedName.size() == 0)
        listUser = User::SearchUserByRole(role);
    else listUser = User::SearchUserBy(name, role);

    refreshStaffDashboard(listUser);
}

void AdminWindow::on_btnEditStaff_clicked() {
    int currentRow = ui->tblStaff->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a staff member from the table to edit.");
        return;
    }

    QString id = ui->tblStaff->item(currentRow, 0)->text();
    QString username = ui->tblStaff->item(currentRow, 1)->text();
    QString fullName = ui->tblStaff->item(currentRow, 2)->text();
    QString phone = ui->tblStaff->item(currentRow, 3)->text();
    QString status = ui->tblStaff->item(currentRow, 4)->text();
    QString role = ui->tblStaff->item(currentRow, 5)->text();

    // Populate overlay fields
    ui->txtEditUsername->setText(username);
    ui->txtEditFullName->setText(fullName);
    ui->txtEditPhone->setText(phone);
    
    // Reset password field and checkbox
    ui->txtEditPassword->clear();
    ui->txtEditPassword->setEnabled(false);
    ui->chkChangePassword->setChecked(false);

    // Set Combobox role
    int roleIdx = ui->cbEditRole->findText(role);
    if (roleIdx >= 0) ui->cbEditRole->setCurrentIndex(roleIdx);

    // Set Combobox status
    int statusIdx = ui->cbEditActive->findText(status);
    if (statusIdx >= 0) ui->cbEditActive->setCurrentIndex(statusIdx);

    // Save selected ID for update operation
    ui->overlayEditStaffFrame->setProperty("selectedStaffId", id.toInt());

    showEditStaffOverlay();
}

void AdminWindow::on_btnCancelEditStaff_clicked() {
    hideEditStaffOverlay();
}

void AdminWindow::on_chkChangePassword_toggled(bool checked) {
    ui->txtEditPassword->setEnabled(checked);
    if (!checked) {
        ui->txtEditPassword->clear();
    }
}

void AdminWindow::on_btnSaveEditStaff_clicked() {
    int id = ui->overlayEditStaffFrame->property("selectedStaffId").toInt();
    QString fullName = ui->txtEditFullName->text().trimmed();
    QString phone = ui->txtEditPhone->text().trimmed();
    QString role = ui->cbEditRole->currentText();
    bool isActive = (ui->cbEditActive->currentText() == "Active");

    QString newPassword = "";
    if (ui->chkChangePassword->isChecked()) {
        newPassword = ui->txtEditPassword->text();
        if (newPassword.isEmpty()) {
            QMessageBox::warning(this, "Validation Error", "Password cannot be empty when changing password.");
            return;
        }
    }

    if (fullName.isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Full Name is required.");
        return;
    }

    bool success = Admin::updateAccount(id, fullName, phone, role, isActive, newPassword);
    if (success) {
        QMessageBox::information(this, "Success", "Staff account updated successfully!");
        
        // Refresh table and stats
        on_btnRefreshStaff_clicked();
        updateDashboardInfo();
        
        hideEditStaffOverlay();
    } else {
        QMessageBox::critical(this, "Error", "Failed to update staff account.");
    }
}

void AdminWindow::showEditStaffOverlay() {
    // 1. Disable background interactions
    ui->bgWidget->setEnabled(false);

    // 2. Set geometry to cover full window size dynamically
    ui->overlayEditStaffFrame->setGeometry(0, 0, this->width(), this->height());
    int cardX = (this->width() - ui->cardEditStaff->width()) / 2;
    int cardY = (this->height() - ui->cardEditStaff->height()) / 2;
    ui->cardEditStaff->move(cardX, cardY);

    // 3. Apply single blur to background container
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(this);
    blur->setBlurRadius(8);
    ui->bgWidget->setGraphicsEffect(blur);
    ui->bgWidget->repaint();

    // 4. Clear graphics effect on overlay
    ui->overlayEditStaffFrame->setGraphicsEffect(nullptr);

    // 5. Show overlay frame
    ui->overlayEditStaffFrame->show();
    ui->overlayEditStaffFrame->raise();

    // 6. Setup opacity animation for smooth fade-in
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(ui->overlayEditStaffFrame);
    ui->overlayEditStaffFrame->setGraphicsEffect(opacityEffect);

    QPropertyAnimation *fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    fadeAnimation->setDuration(100);
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void AdminWindow::hideEditStaffOverlay() {
    // 1. Remove background blur and re-enable background interactions
    ui->bgWidget->setGraphicsEffect(nullptr);
    ui->bgWidget->setEnabled(true);

    // 2. Hide overlay
    ui->overlayEditStaffFrame->hide();
}

void AdminWindow::on_btnDeleteStaff_clicked() {
    int currentRow = ui->tblStaff->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a staff member from the table to delete.");
        return;
    }

    QString id = ui->tblStaff->item(currentRow, 0)->text();
    QString username = ui->tblStaff->item(currentRow, 1)->text();
    QString fullName = ui->tblStaff->item(currentRow, 2)->text();

    // Prevent deletion of self
    if (id.toInt() == User::GetActiveUser().GetID()) {
        QMessageBox::warning(this, "Action Denied", "You cannot delete your own administrator account.");
        return;
    }

    // Set details text in overlay
    ui->lblDeleteStaffDetails->setText(QString("Username: %1\nName:     %2").arg(username).arg(fullName));

    // Save ID for confirm deletion slot
    ui->overlayDeleteStaffFrame->setProperty("deleteStaffId", id.toInt());

    showDeleteStaffOverlay();
}

void AdminWindow::on_btnCancelDeleteStaff_clicked() {
    hideDeleteStaffOverlay();
}

void AdminWindow::on_btnConfirmDeleteStaff_clicked() {
    int id = ui->overlayDeleteStaffFrame->property("deleteStaffId").toInt();

    bool success = Admin::deleteAccount(id);
    if (success) {
        QMessageBox::information(this, "Success", "Staff account deleted successfully!");
        
        // Refresh table and stats
        on_btnRefreshStaff_clicked();
        updateDashboardInfo();
        
        hideDeleteStaffOverlay();
    } else {
        QMessageBox::critical(this, "Error", "Failed to delete staff account.");
    }
}

void AdminWindow::showDeleteStaffOverlay() {
    // 1. Disable background interactions
    ui->bgWidget->setEnabled(false);

    // 2. Set geometry to cover full window size dynamically
    ui->overlayDeleteStaffFrame->setGeometry(0, 0, this->width(), this->height());
    int cardX = (this->width() - ui->cardDeleteStaff->width()) / 2;
    int cardY = (this->height() - ui->cardDeleteStaff->height()) / 2;
    ui->cardDeleteStaff->move(cardX, cardY);

    // 3. Apply single blur to background container
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(this);
    blur->setBlurRadius(8);
    ui->bgWidget->setGraphicsEffect(blur);
    ui->bgWidget->repaint();

    // 4. Clear graphics effect on overlay
    ui->overlayDeleteStaffFrame->setGraphicsEffect(nullptr);

    // 5. Show overlay frame
    ui->overlayDeleteStaffFrame->show();
    ui->overlayDeleteStaffFrame->raise();

    // 6. Setup opacity animation for smooth fade-in
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(ui->overlayDeleteStaffFrame);
    ui->overlayDeleteStaffFrame->setGraphicsEffect(opacityEffect);

    QPropertyAnimation *fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    fadeAnimation->setDuration(100);
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void AdminWindow::hideDeleteStaffOverlay() {
    // 1. Remove background blur and re-enable background interactions
    ui->bgWidget->setGraphicsEffect(nullptr);
    ui->bgWidget->setEnabled(true);

    // 2. Hide overlay
    ui->overlayDeleteStaffFrame->hide();
}

