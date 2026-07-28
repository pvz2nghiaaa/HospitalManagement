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
#include "attendancelog.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QGridLayout>
#include <QSqlDatabase>

AdminWindow::AdminWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    ui->overlayFrame->hide(); // Hide overlay by default
    ui->overlayPatientFrame->hide(); // Hide patient overlay by default
    ui->overlayEditStaffFrame->hide(); // Hide edit staff overlay by default
    ui->overlayDeleteStaffFrame->hide(); // Hide delete staff overlay by default
    ui->overlayEditPermissionFrame->hide(); // Hide edit permission overlay by default
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
    refreshPermissionTable(User::GetAllUserPermission());
}

void AdminWindow::refreshPermissionTable(vector<tuple<int, QString, QString, QList<Permission> > > list){
    // id - full name - role - list permission
    int n = list.size();

    ui->tblPermission->setRowCount(0);
    ui->tblPermission->setAlternatingRowColors(true); // Enable zebra striping
    
    for (int i = 0; i < n; i++){
        ui->tblPermission->insertRow(i);
        
        // Col 0: ID (Primary Style)
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(get<0>(list[i])));
        idItem->setForeground(QColor(241, 245, 249)); // Off-white
        ui->tblPermission->setItem(i, 0, idItem);
        
        // Col 1: Full Name (Primary Style)
        QTableWidgetItem* nameItem = new QTableWidgetItem(get<1>(list[i]));
        nameItem->setForeground(QColor(241, 245, 249)); // Off-white
        ui->tblPermission->setItem(i, 1, nameItem);
        
        // Col 2: Role (Modern translucent Pill Badge)
        QString role = get<2>(list[i]);
        QTableWidgetItem* roleSortItem = new QTableWidgetItem(""); // Set text to empty to prevent selection text overlay
        roleSortItem->setData(Qt::UserRole, role); // Store actual value in UserRole for form reading
        ui->tblPermission->setItem(i, 2, roleSortItem);
        
        QWidget* roleWidget = nullptr;
        if (role == "Admin") {
            roleWidget = createBadgeWidget("Admin", QColor(34, 211, 238), QColor(6, 182, 212));
        } else if (role == "Doctor") {
            roleWidget = createBadgeWidget("Doctor", QColor(45, 212, 191), QColor(45, 212, 191));
        } else {
            roleWidget = createBadgeWidget("Receptionist", QColor(165, 180, 252), QColor(129, 140, 248));
        }
        ui->tblPermission->setCellWidget(i, 2, roleWidget);

        // Col 3: Permissions (Modern visual group-colored chips)
        QList<Permission> perms = get<3>(list[i]);
        QStringList readablePerms;
        for (const Permission &p : perms) {
            readablePerms.push_back(Permission::permissionToReadableString(p));
        }
        QString joinedPerms = readablePerms.isEmpty() ? "No Permissions" : readablePerms.join(", ");
        
        QTableWidgetItem* permsSortItem = new QTableWidgetItem(""); // Set text to empty to prevent selection text overlay
        permsSortItem->setData(Qt::UserRole, joinedPerms); // Store actual value in UserRole
        ui->tblPermission->setItem(i, 3, permsSortItem);
        
        QWidget* chipsWidget = createPermissionChipsWidget(perms);
        ui->tblPermission->setCellWidget(i, 3, chipsWidget);
    }
    ui->tblPermission->resizeRowsToContents(); // adjust its height according to the content
}

void AdminWindow::refreshStaffDashboard(vector<tuple<int, QString, QString, QString, bool, QString>> listUser){
    int n = listUser.size();
    ui->tblStaff->setRowCount(0); // clear old list
    ui->tblStaff->setAlternatingRowColors(true); // Enable zebra striping
    
    for (int i = 0; i < n; i++){
        tuple<int, QString, QString, QString, bool, QString>& user = listUser[i];
        ui->tblStaff->insertRow(i);
        
        // Col 0: ID (Primary Style)
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(get<0>(user)));
        idItem->setForeground(QColor(241, 245, 249)); // Off-white
        ui->tblStaff->setItem(i, 0, idItem);
        
        // Col 1: Username (Secondary Muted Style)
        QTableWidgetItem* userItem = new QTableWidgetItem(get<1>(user));
        userItem->setForeground(QColor(148, 163, 184)); // Muted slate-gray
        ui->tblStaff->setItem(i, 1, userItem);
        
        // Col 2: Full Name (Primary Style)
        QTableWidgetItem* nameItem = new QTableWidgetItem(get<2>(user));
        nameItem->setForeground(QColor(241, 245, 249)); // Off-white
        ui->tblStaff->setItem(i, 2, nameItem);
        
        // Col 3: Phone number (Secondary Muted Style)
        QTableWidgetItem* phoneItem = new QTableWidgetItem(get<3>(user));
        phoneItem->setForeground(QColor(148, 163, 184)); // Muted slate-gray
        ui->tblStaff->setItem(i, 3, phoneItem);
        
        // Col 4: Status (Modern translucent Pill Badge)
        bool isActive = get<4>(user);
        QTableWidgetItem* statusSortItem = new QTableWidgetItem(""); // Set text to empty to prevent selection text overlay
        statusSortItem->setData(Qt::UserRole, isActive ? "Active" : "Inactive"); // Store actual value in UserRole for editing
        ui->tblStaff->setItem(i, 4, statusSortItem);
        
        QWidget* statusWidget = isActive 
            ? createBadgeWidget("Active", QColor(74, 222, 128), QColor(34, 197, 94))
            : createBadgeWidget("Inactive", QColor(248, 113, 113), QColor(239, 68, 68));
        ui->tblStaff->setCellWidget(i, 4, statusWidget);
        
        // Col 5: Role (Modern translucent Pill Badge)
        QString role = get<5>(user);
        QTableWidgetItem* roleSortItem = new QTableWidgetItem(""); // Set text to empty to prevent selection text overlay
        roleSortItem->setData(Qt::UserRole, role); // Store actual value in UserRole for editing
        ui->tblStaff->setItem(i, 5, roleSortItem);
        
        QWidget* roleWidget = nullptr;
        if (role == "Admin") {
            roleWidget = createBadgeWidget("Admin", QColor(34, 211, 238), QColor(6, 182, 212));
        } else if (role == "Doctor") {
            roleWidget = createBadgeWidget("Doctor", QColor(45, 212, 191), QColor(45, 212, 191));
        } else {
            roleWidget = createBadgeWidget("Receptionist", QColor(165, 180, 252), QColor(129, 140, 248));
        }
        ui->tblStaff->setCellWidget(i, 5, roleWidget);
    }
}

void AdminWindow::refreshLogsDashboard(QList<AttendanceLog> list){
    int n = list.size();
    ui->tblActivityLog->setRowCount(0); // clear old list
    ui->tblActivityLog->setAlternatingRowColors(true); // Enable zebra striping

    auto users = User::GetAllUser();

    for (int i = 0; i < n; i++){
        ui->tblActivityLog->insertRow(i);

        int empId = list[i].getEmployeeId();
        QString username = "Unknown";
        QString role = "Unknown";
        for (const auto& u : users) {
            if (get<0>(u) == empId) {
                username = get<1>(u);
                role = get<5>(u);
                break;
            }
        }

        // Col 0: ID (Primary Style)
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(list[i].getId()));
        idItem->setForeground(QColor(241, 245, 249)); // Off-white
        ui->tblActivityLog->setItem(i, 0, idItem);

        // Col 1: Time (Secondary Muted Style)
        QTableWidgetItem* timeItem = new QTableWidgetItem(list[i].getDate());
        timeItem->setForeground(QColor(148, 163, 184)); // Muted slate-gray
        ui->tblActivityLog->setItem(i, 1, timeItem);

        // Col 2: Username (Primary Style)
        QTableWidgetItem* userItem = new QTableWidgetItem(username);
        userItem->setForeground(QColor(241, 245, 249)); // Off-white
        ui->tblActivityLog->setItem(i, 2, userItem);

        // Col 3: Role (Modern translucent Pill Badge)
        QTableWidgetItem* roleSortItem = new QTableWidgetItem(""); // Set text to empty to prevent selection text overlay
        roleSortItem->setData(Qt::UserRole, role); // Store actual value in UserRole for editing
        ui->tblActivityLog->setItem(i, 3, roleSortItem);

        QWidget* roleWidget = nullptr;
        if (role == "Admin") {
            roleWidget = createBadgeWidget("Admin", QColor(34, 211, 238), QColor(6, 182, 212));
        } else if (role == "Doctor") {
            roleWidget = createBadgeWidget("Doctor", QColor(45, 212, 191), QColor(45, 212, 191));
        } else {
            roleWidget = createBadgeWidget("Receptionist", QColor(165, 180, 252), QColor(129, 140, 248));
        }
        ui->tblActivityLog->setCellWidget(i, 3, roleWidget);

        // Col 4: Action (Modern translucent Pill Badge)
        bool isPresent = list[i].getIsPresent();
        QTableWidgetItem* actionSortItem = new QTableWidgetItem("");
        actionSortItem->setData(Qt::UserRole, isPresent ? "Present" : "Absent");
        ui->tblActivityLog->setItem(i, 4, actionSortItem);

        QWidget* actionWidget = isPresent
                                    ? createBadgeWidget("Present", QColor(74, 222, 128), QColor(34, 197, 94))
                                    : createBadgeWidget("Absent", QColor(248, 113, 113), QColor(239, 68, 68));
        ui->tblActivityLog->setCellWidget(i, 4, actionWidget);

        // Col 5: Description (Secondary Muted Style)
        QString desc = isPresent ? "Employee checked in / Present" : "Employee marked as Absent";
        QTableWidgetItem* descItem = new QTableWidgetItem(desc);
        descItem->setForeground(QColor(148, 163, 184)); // Muted slate-gray
        ui->tblActivityLog->setItem(i, 5, descItem);
    }
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

    refreshLogsDashboard(AttendanceLog::GetRecentLogs());
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
    QString birthDate = ui->datePatDOB->date().toString("dd-MM-yyyy");
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
        else listUser = User::SearchUserBy(name); // remove trim
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
    QString status = ui->tblStaff->item(currentRow, 4)->data(Qt::UserRole).toString(); // Read from UserRole instead of visible text
    QString role = ui->tblStaff->item(currentRow, 5)->data(Qt::UserRole).toString();   // Read from UserRole instead of visible text

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


void AdminWindow::on_btnSearchPermission_clicked()
{

    vector<tuple<int, QString, QString, QList<Permission> > > listPermission;
    QString name = ui->txtSearchPermission->text();
    QString trimmedName = name.trimmed();
    QString role = ui->cbRolePermission->currentText();
    if (role == "All"){
        if (trimmedName.size() == 0){
            listPermission = User::GetAllUserPermission();
        }
        else listPermission = User::SearchPermissionBy(name); // remove trim
    }
    else if (trimmedName.size() == 0)
        listPermission = User::SearchPermissionByRole(role);
    else listPermission = User::SearchPermissionBy(name, role);

    refreshPermissionTable(listPermission);
}

QWidget* AdminWindow::createBadgeWidget(const QString& text, const QColor& textColor, const QColor& bgColor) {
    QWidget* container = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);

    QLabel* label = new QLabel(text);
    label->setStyleSheet(QString(
        "QLabel {"
        "  color: %1;"
        "  background-color: rgba(%2, %3, %4, 0.15);"
        "  border-radius: 10px;"
        "  padding: 4px 10px;"
        "  font-size: 11px;"
        "  font-weight: bold;"
        "}"
    ).arg(textColor.name(QColor::HexRgb))
     .arg(bgColor.red()).arg(bgColor.green()).arg(bgColor.blue()));

    layout->addWidget(label);
    container->setLayout(layout);
    return container;
}

QWidget* AdminWindow::createPermissionChipsWidget(const QList<Permission>& perms) {
    QWidget* container = new QWidget();
    QGridLayout* layout = new QGridLayout(container);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->setHorizontalSpacing(6);
    layout->setVerticalSpacing(4);

    int row = 0;
    int col = 0;
    const int maxCols = 2; // Lay chips out in 2 columns maximum to prevent narrowing and fit nicely in row height
    
    for (const Permission& p : perms) {
        Permission::Type type = static_cast<Permission::Type>(p);
        QString labelText = Permission::permissionToReadableString(p);
        
        QColor textColor;
        QColor bgColor;

        // Group colors
        switch (type) {
            // Purple for System Management
            case Permission::viewLog:
            case Permission::addLog:
            case Permission::changePermission:
            case Permission::manageUsers:
                textColor = QColor(216, 180, 254);
                bgColor = QColor(168, 85, 247);
                break;
                
            // Blue for Medical Records
            case Permission::createRecord:
            case Permission::viewRecord:
            case Permission::editRecord:
                textColor = QColor(147, 197, 253);
                bgColor = QColor(59, 130, 246);
                break;
                
            // Emerald Green for Patients
            case Permission::createPatient:
            case Permission::editPatient:
                textColor = QColor(110, 231, 183);
                bgColor = QColor(16, 185, 129);
                break;
                
            // Amber for Billing
            case Permission::createInvoice:
            case Permission::viewInvoice:
                textColor = QColor(253, 230, 138);
                bgColor = QColor(245, 158, 11);
                break;
                
            // Pink for Drugs/Services
            case Permission::manageDrugs:
                textColor = QColor(249, 168, 212);
                bgColor = QColor(236, 72, 153);
                break;
                
            default:
                textColor = QColor(241, 245, 249);
                bgColor = QColor(148, 163, 184);
                break;
        }

        QLabel* chip = new QLabel(labelText);
        chip->setAlignment(Qt::AlignCenter);
        chip->setStyleSheet(QString(
            "QLabel {"
            "  color: %1;"
            "  background-color: rgba(%2, %3, %4, 0.15);"
            "  border-radius: 4px;"
            "  padding: 2px 6px;"
            "  font-size: 11px;"
            "  font-weight: bold;"
            "}"
        ).arg(textColor.name(QColor::HexRgb))
         .arg(bgColor.red()).arg(bgColor.green()).arg(bgColor.blue()));

        layout->addWidget(chip, row, col);
        
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
    
    // Fallback if no permissions
    if (perms.isEmpty()) {
        QLabel* emptyLabel = new QLabel("No Permissions");
        emptyLabel->setStyleSheet("color: rgb(100, 116, 139); font-size: 11px; font-style: italic;");
        layout->addWidget(emptyLabel, 0, 0);
    }

    container->setLayout(layout);
    return container;
}

void AdminWindow::on_btnEditPermission_clicked() {
    int currentRow = ui->tblPermission->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "No Selection", "Please select a user from the table to edit permissions.");
        return;
    }

    QString id = ui->tblPermission->item(currentRow, 0)->text();
    QString fullName = ui->tblPermission->item(currentRow, 1)->text();
    QString role = ui->tblPermission->item(currentRow, 2)->data(Qt::UserRole).toString();

    // Fetch username from DB via User class
    QString username = User::GetUsernameById(id.toInt());

    // Set details text in overlay
    ui->lblEditPermissionDetails->setText(QString("ID:       %1\nUsername: %2\nName:     %3\nRole:     %4")
        .arg(id).arg(username).arg(fullName).arg(role));

    // Get active user permissions
    QList<Permission> perms = Permission::GetUserPermission(id.toInt());

    // Update checkboxes
    ui->chkPermViewLog->setChecked(perms.contains(Permission::viewLog));
    ui->chkPermAddLog->setChecked(perms.contains(Permission::addLog));
    ui->chkPermChangePermission->setChecked(perms.contains(Permission::changePermission));
    ui->chkPermManageUsers->setChecked(perms.contains(Permission::manageUsers));
    ui->chkPermCreateRecord->setChecked(perms.contains(Permission::createRecord));
    ui->chkPermViewRecord->setChecked(perms.contains(Permission::viewRecord));
    ui->chkPermEditRecord->setChecked(perms.contains(Permission::editRecord));
    ui->chkPermCreatePatient->setChecked(perms.contains(Permission::createPatient));
    ui->chkPermEditPatient->setChecked(perms.contains(Permission::editPatient));
    ui->chkPermCreateInvoice->setChecked(perms.contains(Permission::createInvoice));
    ui->chkPermViewInvoice->setChecked(perms.contains(Permission::viewInvoice));
    ui->chkPermManageDrugs->setChecked(perms.contains(Permission::manageDrugs));

    // Save ID for saving/confirming slot
    ui->overlayEditPermissionFrame->setProperty("selectedUserId", id.toInt());

    showEditPermissionOverlay();
}

void AdminWindow::on_btnCancelEditPermission_clicked() {
    hideEditPermissionOverlay();
}

void AdminWindow::on_btnSaveEditPermission_clicked() {
    int userID = ui->overlayEditPermissionFrame->property("selectedUserId").toInt();
    QList<Permission> updatedPerms;
    QList<Permission> previousPerms = Permission::GetUserPermission(userID);
    // why am i doing this
    if (ui->chkPermViewLog->isChecked())           updatedPerms.append(Permission::viewLog);
    if (ui->chkPermAddLog->isChecked())            updatedPerms.append(Permission::addLog);
    if (ui->chkPermChangePermission->isChecked())  updatedPerms.append(Permission::changePermission);
    if (ui->chkPermManageUsers->isChecked())       updatedPerms.append(Permission::manageUsers);
    if (ui->chkPermCreateRecord->isChecked())      updatedPerms.append(Permission::createRecord);
    if (ui->chkPermViewRecord->isChecked())        updatedPerms.append(Permission::viewRecord);
    if (ui->chkPermEditRecord->isChecked())        updatedPerms.append(Permission::editRecord);
    if (ui->chkPermCreatePatient->isChecked())      updatedPerms.append(Permission::createPatient);
    if (ui->chkPermEditPatient->isChecked())        updatedPerms.append(Permission::editPatient);
    if (ui->chkPermCreateInvoice->isChecked())      updatedPerms.append(Permission::createInvoice);
    if (ui->chkPermViewInvoice->isChecked())        updatedPerms.append(Permission::viewInvoice);
    if (ui->chkPermManageDrugs->isChecked())       updatedPerms.append(Permission::manageDrugs);

    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    bool success = true;

    for (Permission perms: previousPerms)
        if (!updatedPerms.contains(perms))
            success &= Permission::changeUserPermission(userID, perms, false);

    for (Permission perms: updatedPerms)
        if (!previousPerms.contains(perms))
            success &= Permission::changeUserPermission(userID, perms, true);

    if (success) {
        db.commit();
        if (userID == User::GetActiveUser().GetID()) {
            User::GetActiveUser().UpdatePermissionFromDatabase();
        }
        QMessageBox::information(this, "Success", "Permissions updated successfully!");
    } else {
        db.rollback();
        QMessageBox::critical(this, "Failure", "Permission cannot be updated due to database error.");
    }
    hideEditPermissionOverlay();
    refreshPermissionTable(User::GetAllUserPermission());
}

void AdminWindow::showEditPermissionOverlay() {
    // 1. Disable background interactions
    ui->bgWidget->setEnabled(false);

    // 2. Set geometry to cover full window size dynamically
    ui->overlayEditPermissionFrame->setGeometry(0, 0, this->width(), this->height());
    int cardX = (this->width() - ui->cardEditPermission->width()) / 2;
    int cardY = (this->height() - ui->cardEditPermission->height()) / 2;
    ui->cardEditPermission->move(cardX, cardY);

    // 3. Apply single blur to background container
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect(this);
    blur->setBlurRadius(8);
    ui->bgWidget->setGraphicsEffect(blur);
    ui->bgWidget->repaint();

    // 4. Clear graphics effect on overlay
    ui->overlayEditPermissionFrame->setGraphicsEffect(nullptr);

    // 5. Show overlay frame
    ui->overlayEditPermissionFrame->show();
    ui->overlayEditPermissionFrame->raise();

    // 6. Setup opacity animation for smooth fade-in
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(ui->overlayEditPermissionFrame);
    ui->overlayEditPermissionFrame->setGraphicsEffect(opacityEffect);

    QPropertyAnimation *fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    fadeAnimation->setDuration(100);
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void AdminWindow::hideEditPermissionOverlay() {
    // 1. Remove background blur and re-enable background interactions
    ui->bgWidget->setGraphicsEffect(nullptr);
    ui->bgWidget->setEnabled(true);

    // 2. Hide overlay
    ui->overlayEditPermissionFrame->hide();
}

