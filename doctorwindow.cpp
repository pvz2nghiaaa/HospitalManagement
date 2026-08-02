#include "doctorwindow.h"
#include "ui_DoctorWindow.h"
#include "user.h"
#include "loginwindow.h"
#include "doctor.h"

#include <QDate>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QCompleter>
#include <QComboBox>


DoctorWindow::DoctorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DoctorWindow)
{


    ui->setupUi(this);

    ui->dateIssued->setDisplayFormat("dd-MM-yyyy");
    ui->dateIssued->setDate(QDate::currentDate());

    ui->lblAdmin->setText("Dr. " + User::GetActiveUser().GetFullName() + " (Online)");
    navigateToPage(0, ui->btnDashboard);

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

void DoctorWindow::on_btnSearch_4_clicked() {
    QString keyword = ui->txtSearch_4->text();
    currentPatientList = Doctor::SearchPatientBy(keyword);

    ui->tblPatient->setRowCount(currentPatientList.size());

    for (int i = 0; i < currentPatientList.size(); ++i) {
        const Patient& p = currentPatientList[i];
        ui->tblPatient->setItem(i, 0, new QTableWidgetItem(QString::number(p.ID)));
        ui->tblPatient->setItem(i, 1, new QTableWidgetItem(p.FullName));
        // ui->tblPatient->setItem(i, 2, new QTableWidgetItem(p.Phone));
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

    QDate dob = QDate::fromString(p.BirthDate, "dd-MM-year");
    if (dob.isValid()) {
        ui->dateDOB->setDate(dob);
    }

    if (p.Sex == "Male" || p.Sex == "Nam") {
        ui->cbGender->setCurrentIndex(0);
    } else {
        ui->cbGender->setCurrentIndex(1);
    }
    currentRecordId = p.ID;
    qDebug() << "Patient with ID:" << currentRecordId;
}

void DoctorWindow::on_btnUpdatePatient_clicked() {
    int id = ui->txtPatientID->text().toInt();
    QString name = ui->txtFullName->text();
    QString dob = ui->dateDOB->date().toString("dd-MM-year");
    QString sex = ui->cbGender->currentText();
    QString address = ui->txtAddress->text();

    if (Doctor::UpdatePatientInfo(id, name, dob, sex, address)) {
        QMessageBox::information(this, "Success", "Update successful!");
        on_btnSearch_4_clicked();
    } else {
        QMessageBox::warning(this, "Error", "Update failed! Check permissions or connection.");
    }
}

void DoctorWindow::on_btnNewDisease_clicked()
{
    bool okName, okCode;
    QString newName = QInputDialog::getText(this, "Add new disease", "Codition name: ", QLineEdit::Normal, "", &okName);
    if (!okName || newName.trimmed().isEmpty()) return;
    QString newCode = QInputDialog::getText(this, "Add new disease", "Codition code(ICD): ", QLineEdit::Normal, "", &okCode);
    if (!okCode || newCode.trimmed().isEmpty()) return;

    if (Doctor::AddNewDisease(newName.trimmed(), newCode.trimmed())){
        QMessageBox::information(this, "Successfully", "Added new disease successfully!");
        ui->cbDisease->clear();
        ui->cbDisease->addItems(Doctor::GetDiseasesList());
        ui->cbDisease->setCurrentText(newName.trimmed());
    }
    else
        QMessageBox::warning(this, "Error", "Can not add new disease. Check again!!");
}


void DoctorWindow::on_btnSaveDiagnosis_clicked()
{
    if (currentRecordId <= 0) {
        QMessageBox::warning(this, "Warning", "Please select a patient from the list before saving!");
        return;
    }

    QString conditionName = ui->cbDisease->currentText();
    QString icdCode = ui->lineEdit->text();
    QString severity = ui->cbSeverity->currentText();
    QString clinicalNote = ui->textEdit->toPlainText();
    QString dateDiagnosed = ui->dateEdit->date().toString("yyyy-MM-dd");

    if (conditionName.isEmpty() || icdCode.isEmpty()) {
        QMessageBox::warning(this, "Insufficient information", "Please select the disease name and ensure you have the ICD code!");
        return;
    }
        bool isSuccess = Doctor::SaveDiagnosis(currentRecordId, conditionName, icdCode, severity, clinicalNote, dateDiagnosed);

    if (isSuccess) {
        QMessageBox::information(this, "Success", "Diagnosis successfully saved!");

    } else {
        QMessageBox::critical(this, "Database error", "Diagnosis saving failed! Please check the system again.");
    }
}


void DoctorWindow::on_btnComplete_clicked()
{
    if (currentRecordId <= 0)
    {
        QMessageBox::warning(this, "Warning", "Please select a patient from the list before completing the record!");
        return;
    }
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation",
    "Are you sure you want to mark this record as complete? Once completed, it cannot be edited.",
    QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No)
    {
        return;
    }
    if (Doctor::MarkRecordComplete(currentRecordId))
    {
        QMessageBox::information(this, "Success", "Record marked as complete successfully!");
        ui->cbDisease->setCurrentIndex(0);
        ui->lineEdit->clear();
        ui->cbSeverity->setCurrentIndex(0);
        ui->textEdit->clear();

    }
    else
    {
        QMessageBox::critical(this, "Database Error", "Failed to save! Unable to complete the record, please check your connection.");
    }
}

void DoctorWindow::on_btnRemoveDrug_clicked()
{
    int currentRow = ui->tblPrescription->currentRow();

    if (currentRow < 0) {
        QMessageBox::warning(this, "Warning", "Please select a drug from the table to remove!");
        return;
    }

    ui->tblPrescription->removeRow(currentRow);
}

void DoctorWindow::on_btnAddDrug_clicked()
{
    if (currentRecordId == -1) {
        QMessageBox::warning(this, "Warning", "Please select a medical record first!");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Add New Drug");
    dialog.setMinimumWidth(350);

    QFormLayout layout(&dialog);

    // 1. TẠO COMBOBOX VÀ NẠP DANH SÁCH TÊN THUỐC
    QComboBox cbDrug(&dialog);
    cbDrug.setEditable(true); // Biến ComboBox thành thanh gõ chữ được
    cbDrug.setInsertPolicy(QComboBox::NoInsert);

    QList<QString> drugList = Doctor::GetDrugsList();
    cbDrug.addItems(drugList);

    // Gắn tính năng Search (QCompleter)
    QCompleter* completer = new QCompleter(QStringList(drugList), &dialog);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive); // Gõ chữ hoa/thường đều tìm được
    completer->setFilterMode(Qt::MatchContains); // Gõ từ ở giữa (VD: "mox") vẫn ra "Amoxicillin"
    cbDrug.setCompleter(completer);

    // 2. TẠO CÁC Ô NHẬP SỐ LƯỢNG VÀ HƯỚNG DẪN
    QSpinBox spinQuantity(&dialog);
    spinQuantity.setRange(1, 999);
    spinQuantity.setValue(1);

    QLineEdit txtInstruction(&dialog);
    txtInstruction.setPlaceholderText("e.g. Take 2 times a day");

    layout.addRow("Search Drug:", &cbDrug);
    layout.addRow("Quantity:", &spinQuantity);
    layout.addRow("Instruction:", &txtInstruction);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    layout.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);


    if (dialog.exec() == QDialog::Accepted) {
        QString selectedDrugName = cbDrug.currentText();

        // Gọi hàm phụ để lấy ID từ Tên Thuốc
        int drugId = Doctor::GetDrugIdByName(selectedDrugName);

        if (drugId <= 0) {
            QMessageBox::warning(this, "Warning", "Drug not found! Please select a valid drug from the list.");
            return;
        }

        int quantity = spinQuantity.value();
        QString instruction = txtInstruction.text();

        // K lưu vào db
        int row = ui->tblPrescription->rowCount();
        ui->tblPrescription->insertRow(row);

        ui->tblPrescription->setItem(row, 0, new QTableWidgetItem(QString::number(drugId)));
        ui->tblPrescription->setItem(row, 1, new QTableWidgetItem(selectedDrugName));
        ui->tblPrescription->setItem(row, 3, new QTableWidgetItem(QString::number(quantity)));
        ui->tblPrescription->setItem(row, 5, new QTableWidgetItem(instruction));
    }
}

void DoctorWindow::on_btnSavePrescription_clicked()
{
    if (currentRecordId == -1) {
        QMessageBox::warning(this, "Warning", "Please select a medical record before saving the prescription!");
        return;
    }

    QString dateIssued = ui->dateIssued->date().toString("dd-MM-yyyy");

    QList<Drug> drugsList;
    QList<int> quantitiesList;
    QList<QString> notesList;

    int rowCount = ui->tblPrescription->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        if (!ui->tblPrescription->item(i, 0)) continue;

        int drugId = ui->tblPrescription->item(i, 0)->text().toInt();

        if (drugId <= 0) continue;

        int quantity = ui->tblPrescription->item(i, 3) ? ui->tblPrescription->item(i, 3)->text().toInt() : 1;
        QString note = ui->tblPrescription->item(i, 5) ? ui->tblPrescription->item(i, 5)->text() : "";

        Drug d;
        d.setDrugID(drugId);

        drugsList.append(d);
        quantitiesList.append(quantity);
        notesList.append(note);
    }

    Prescription finalPrescription;
    finalPrescription.setDiagnosisID(currentRecordId);
    finalPrescription.setDrugs(drugsList);
    finalPrescription.setQuantities(quantitiesList);
    finalPrescription.setNotes(notesList);

    QList<Prescription> itemsToSave;
    itemsToSave.append(finalPrescription);

    if (Doctor::SavePrescription(currentRecordId, dateIssued, itemsToSave)) {
        QMessageBox::information(this, "Success", "Prescription saved successfully!");
    } else {
        QMessageBox::critical(this, "Error", "An error occurred while saving the prescription.");
    }
}