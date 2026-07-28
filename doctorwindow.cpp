#include "doctorwindow.h"
#include "ui_DoctorWindow.h"
#include "user.h"
#include "doctor.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QDate>
#include "loginwindow.h"
#include <QFileDialog>

DoctorWindow::DoctorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DoctorWindow)
{
    ui->setupUi(this);
    ui->lblAdmin->setText("Dr. " + User::GetActiveUser().GetFullName() + " (Online)");

    ui->lineEdit_2->setReadOnly(true); // Doctor ID
    ui->lineEdit_3->setReadOnly(true); // Full Name
    ui->lineEdit_4->setReadOnly(true); // Gender
    ui->lineEdit_5->setReadOnly(true); // Date of Birth
    ui->lineEdit_6->setReadOnly(true); // Phone Number
    ui->tblMyActivity->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblMyPermissions->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->dateEdit_2->setDisplayFormat("dd-MM-yyyy");
    ui->dateEdit_3->setDisplayFormat("dd-MM-yyyy");
    ui->dateEdit_2->setDate(QDate::currentDate().addDays(-30));
    ui->dateEdit_3->setDate(QDate::currentDate());

    loadProfileData();
    navigateToPage(0, ui->btnDashboard);

    ui->txtRecordID->setReadOnly(true);
    ui->txtRecordID_2->setReadOnly(true);
    ui->txtRecordID_4->setReadOnly(true);
    ui->txtRecordID_3->setReadOnly(true);
    ui->txtRecordID_5->setReadOnly(true);

    ui->tblRecordPrescription->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblMedicalRecords->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tblPatient->setColumnCount(6);
    ui->tblPatient->setHorizontalHeaderLabels({"ID", "Full Name", "Phone", "BirthDate", "Sex", "Address"});
    ui->tblPatient->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

DoctorWindow::~DoctorWindow()
{
    delete ui;
}

void DoctorWindow::on_btnDashboard_clicked()
{
    navigateToPage(0, ui->btnDashboard);
}

void DoctorWindow::on_btnPatients_clicked()
{
    navigateToPage(1, ui->btnPatients);
}

void DoctorWindow::on_btnDoctors_clicked()
{
    navigateToPage(2, ui->btnDoctors);
}

void DoctorWindow::navigateToPage(int pageIndex, QPushButton* activeBtn){
    ui->stackedWidget->setCurrentIndex(pageIndex);
    QList<QPushButton*> sidebarButtons = {
        ui->btnDashboard,
        ui->btnPatients,
        ui->btnDoctors
    };
    for (QPushButton* btn: sidebarButtons){
        btn->setProperty("active", false);
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }

    activeBtn->setProperty("active", true);
    activeBtn->style()->unpolish(activeBtn);
    activeBtn->style()->polish(activeBtn);
}

void DoctorWindow::on_btnLogout_clicked()
{
    User::logout();
    QMessageBox::information(this, "Program info", "Logged out successfully!");

    LoginWindow *loginWin = new LoginWindow();
    loginWin->setAttribute(Qt::WA_DeleteOnClose);
    loginWin->show();

    this->close();
}

void DoctorWindow::on_btnSearch_5_clicked()
{
    QString keyword = ui->txtSearch_5->text();
    QString status = ui->cbStatusFilter->currentText();

    qDebug() << "=== ĐANG CHẠY HÀM SEARCH ===";
    qDebug() << "Keyword:" << keyword << "| Status:" << status;

    QList<MedicalRecord> records = Doctor::SearchRecordsBy(keyword, status);

    qDebug() << "Số bệnh án tìm thấy trong DB:" << records.size();

    ui->tblMedicalRecords->setRowCount(0);

    for (int i = 0; i < records.size(); ++i) {
        ui->tblMedicalRecords->insertRow(i);
        MedicalRecord rec = records[i];

        ui->tblMedicalRecords->setItem(i, 0, new QTableWidgetItem(QString::number(rec.GetRecordID())));
        ui->tblMedicalRecords->setItem(i, 1, new QTableWidgetItem(QString::number(rec.GetPatientID())));
        ui->tblMedicalRecords->setItem(i, 2, new QTableWidgetItem(rec.GetDate()));

        ui->tblMedicalRecords->setItem(i, 3, new QTableWidgetItem(rec.GetDoctorName()));
        ui->tblMedicalRecords->setItem(i, 4, new QTableWidgetItem(rec.GetDiagnosis()));

        QString statusText = rec.GetIsComplete() ? "Completed" : "Pending";
        ui->tblMedicalRecords->setItem(i, 5, new QTableWidgetItem(statusText));

        ui->tblMedicalRecords->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        ui->tblMedicalRecords->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        ui->tblMedicalRecords->item(i, 2)->setTextAlignment(Qt::AlignCenter);
        ui->tblMedicalRecords->item(i, 5)->setTextAlignment(Qt::AlignCenter);
    }
}

void DoctorWindow::on_btnRefreshRecords_clicked()
{
    ui->txtSearch_5->clear();
    ui->cbStatusFilter->setCurrentIndex(0);

    on_btnSearch_5_clicked();
}

void DoctorWindow::on_tblMedicalRecords_cellClicked(int row, int column)
{
    QTableWidgetItem* idItem = ui->tblMedicalRecords->item(row, 0);
    if (!idItem) return;
    int recordId = idItem->text().toInt();

    MedicalRecord rec = Doctor::GetRecordDetails(recordId);

    ui->txtRecordID->setText(QString::number(rec.GetPatientID()));

    QDate visitDate = QDate::fromString(rec.GetDate(), "dd-MM-yyyy");
    ui->dateEdit->setDate(visitDate);

    QString patientName, doctorName, diagnosis;
    Doctor::GetRecordExtraInfo(recordId, patientName, doctorName, diagnosis);

    ui->txtRecordID_2->setText(patientName);
    ui->txtRecordID_3->setText(doctorName);
    ui->txtRecordID_5->setText(diagnosis);

    QList<Prescription> rxList = Doctor::GetRecordPrescriptions(recordId);
    ui->tblRecordPrescription->setRowCount(0);

    int r = 0;
    for (const Prescription& rx : rxList) {
        QList<Drug> drugs = rx.getDrugs();
        QList<int> qtys = rx.getQuantities();

        for (int i = 0; i < drugs.size(); ++i) {
            ui->tblRecordPrescription->insertRow(r);

            ui->tblRecordPrescription->setItem(r, 0, new QTableWidgetItem(drugs[i].getName()));

            QString qtyText = QString::number(qtys[i]) + " " + drugs[i].getUnit();
            QTableWidgetItem *itemQty = new QTableWidgetItem(qtyText);
            itemQty->setTextAlignment(Qt::AlignCenter);
            ui->tblRecordPrescription->setItem(r, 1, itemQty);
            r++;
        }
    }
}

void DoctorWindow::on_btnPrintRecord_clicked()
{
    int row = ui->tblMedicalRecords->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Warning", "Please select a medical record from the list to print!");
        return;
    }

    QTableWidgetItem* statusItem = ui->tblMedicalRecords->item(row, 5);
    if (statusItem && statusItem->text() == "Pending") {
        QMessageBox::warning(this, "Warning", "Cannot print a Pending medical record. Please complete it first!");
        return;
    }

    QTableWidgetItem* idItem = ui->tblMedicalRecords->item(row, 0);
    if (!idItem) return;
    int recordId = idItem->text().toInt();

    QString defaultFileName = "MedicalRecord_" + QString::number(recordId) + ".pdf";
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    "Save Medical Record PDF",
                                                    defaultFileName,
                                                    "PDF Files (*.pdf)");

    if (filePath.isEmpty()) {
        return;
    }

    Doctor::PrintRecord(recordId, filePath);

    QMessageBox::information(this, "Success", "PDF exported successfully at:\n" + filePath);
}
void DoctorWindow::on_btnCloseRecord_clicked()
{
    ui->txtRecordID->clear();
    ui->txtRecordID_2->clear();
    ui->txtRecordID_3->clear();
    ui->txtRecordID_4->clear();
    ui->txtRecordID_5->clear();

    ui->tblRecordPrescription->setRowCount(0);

    ui->tblMedicalRecords->clearSelection();
    ui->tblMedicalRecords->setCurrentItem(nullptr);

}void DoctorWindow::on_btnSearch_4_clicked() {
    QString keyword = ui->txtSearch_4->text();
    currentPatientList = Doctor::SearchPatientBy(keyword);

    ui->tblPatient->setRowCount(currentPatientList.size());

    for (int i = 0; i < currentPatientList.size(); ++i) {
        const Patient& p = currentPatientList[i];
        ui->tblPatient->setItem(i, 0, new QTableWidgetItem(QString::number(p.ID)));
        ui->tblPatient->setItem(i, 1, new QTableWidgetItem(p.FullName));
        ui->tblPatient->setItem(i, 2, new QTableWidgetItem(p.Phone));
        ui->tblPatient->setItem(i, 3, new QTableWidgetItem(p.BirthDate));
        ui->tblPatient->setItem(i, 4, new QTableWidgetItem(p.Sex));
        ui->tblPatient->setItem(i, 5, new QTableWidgetItem(p.Address));
    }
}

void DoctorWindow::on_tblPatient_cellClicked(int row, int column) {
    if (row < 0 || row >= currentPatientList.size()) return;

    const Patient& p = currentPatientList[row];

    ui->txtPatientID->setText(QString::number(p.ID));
    ui->txtFullName->setText(p.FullName);
    ui->txtAddress->setText(p.Address);

    QDate dob = QDate::fromString(p.BirthDate, "yyyy-MM-dd");
    if (dob.isValid()) {
        ui->dateDOB->setDate(dob);
    }

    if (p.Sex == "Male" || p.Sex == "Nam") {
        ui->cbGender->setCurrentIndex(0);
    } else {
        ui->cbGender->setCurrentIndex(1);
    }
}

void DoctorWindow::on_btnUpdatePatient_clicked() {
    int id = ui->txtPatientID->text().toInt();
    QString name = ui->txtFullName->text();
    QString dob = ui->dateDOB->date().toString("yyyy-MM-dd");
    QString sex = ui->cbGender->currentText();
    QString address = ui->txtAddress->text();

    if (Doctor::UpdatePatientInfo(id, name, dob, sex, address)) {
        QMessageBox::information(this, "Success", "Update successful!");
        on_btnSearch_4_clicked();
    } else {
        QMessageBox::warning(this, "Error", "Update failed! Check permissions or connection.");
    }
}
void DoctorWindow::on_btnSearchActivity_clicked()
{
    QString dateFrom = ui->dateEdit_2->date().toString("yyyy-MM-dd");
    QString dateTo = ui->dateEdit_3->date().toString("yyyy-MM-dd");

    ui->dateEdit_2->date().toString("dd-MM-yyyy");
    ui->dateEdit_3->date().toString("dd-MM-yyyy");
    QList<AttendanceLog> logs = Doctor::SearchMyActivityLogs(dateFrom, dateTo);

    ui->tblMyActivity->setRowCount(0);
    ui->tblMyActivity->setRowCount(logs.size());

    for (int i = 0; i < logs.size(); i++) {

        QString fullDateString = logs[i].getDate();
        QDate parsedDate = QDate::fromString(fullDateString, "yyyy-MM-dd");
        QString displayDate = parsedDate.isValid() ? parsedDate.toString("dd-MM-yyyy") : fullDateString;

        QTableWidgetItem *itemDate = new QTableWidgetItem(displayDate);
        QTableWidgetItem *itemTime = new QTableWidgetItem("-");
        QTableWidgetItem *itemAction = new QTableWidgetItem("Attendance");

        QString status = (logs[i].getIsPresent() == 1) ? "Present" : "Absent";
        QTableWidgetItem *itemDesc = new QTableWidgetItem(status);

        itemDate->setTextAlignment(Qt::AlignCenter);
        itemTime->setTextAlignment(Qt::AlignCenter);
        itemAction->setTextAlignment(Qt::AlignCenter);
        itemDesc->setTextAlignment(Qt::AlignCenter);

        ui->tblMyActivity->setItem(i, 0, itemDate);
        ui->tblMyActivity->setItem(i, 1, itemTime);
        ui->tblMyActivity->setItem(i, 2, itemAction);
        ui->tblMyActivity->setItem(i, 3, itemDesc);
    }
}


void DoctorWindow::loadProfileData()
{
    User& myProfile = Doctor::GetMyProfileInfo();
    ui->lineEdit_2->setText(QString::number(myProfile.GetID()));
    ui->lineEdit_3->setText(myProfile.GetFullName());
    ui->lineEdit_4->setText("N/A");
    ui->lineEdit_5->setText("N/A");
    ui->lineEdit_6->setText(myProfile.GetPhoneNumber());
    ui->lineEdit_7->setText(myProfile.GetRole());


    QList<Permission> myPerms = Doctor::GetMyPermissions();
    ui->tblMyPermissions->setRowCount(0);
    ui->tblMyPermissions->setRowCount(myPerms.size());

    QString permissionsText = "";

    for (int i = 0; i < myPerms.size(); ++i) {
        int pType = myPerms[i].toUnderlying(); 
        
        QString pName = "Unknown Permission";
        switch (pType) {
            case Permission::createRecord:
                pName = "Create Medical Record"; 
                break;
            case Permission::viewRecord:
                pName = "View Medical Record"; 
                break;
            case Permission::editRecord:
                pName = "Edit Medical Record"; 
                break;
            case Permission::manageDrugs:
                pName = "Manage Drugs";
                break;
            case Permission::viewLog:
                pName = "View Logs"; 
                break;
            case Permission::addLog:
                pName = "Add Logs";
                break;
            case Permission::changePermission:
                pName = "Change Permissions"; 
                break;
            case Permission::manageUsers:
                pName = "Manage Users"; 
                break;
            case Permission::createPatient:
                pName = "Create Patient"; 
                break;
            case Permission::editPatient:
                pName = "Edit Patient"; 
                break;
            case Permission::createInvoice:
                pName = "Create Invoice"; 
                break;
            case Permission::viewInvoice:
                pName = "View Invoice"; 
                break;
        }

        permissionsText += "- " + pName + "\n";
        QTableWidgetItem *itemPerm = new QTableWidgetItem(pName);
        QTableWidgetItem *itemStatus = new QTableWidgetItem("Granted");
        itemStatus->setTextAlignment(Qt::AlignCenter);

        ui->tblMyPermissions->setItem(i, 0, itemPerm);
        ui->tblMyPermissions->setItem(i, 1, itemStatus);
    }

}
