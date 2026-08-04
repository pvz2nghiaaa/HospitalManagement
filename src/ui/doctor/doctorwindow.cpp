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
#include <QFileDialog>

DoctorWindow::DoctorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DoctorWindow)
{
    ui->setupUi(this);
    ui->dateIssued->setDate(QDate::currentDate());

    QHeaderView *header = ui->tblPatient->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    header->resizeSection(0, 70);
    header->setSectionResizeMode(1, QHeaderView::Stretch);

    ui->lblAdmin->setText("Dr. " + User::GetActiveUser().GetFullName() + " (Online)");
    navigateToPage(0, ui->btnDashboard);

    ui->tblRecordPrescription->setWordWrap(true);

    ui->lineEdit_2->setReadOnly(true); // Doctor ID
    ui->lineEdit_3->setReadOnly(true); // Full Name
    ui->lineEdit_4->setReadOnly(true); // Gender
    ui->lineEdit_5->setReadOnly(true); // Date of Birth
    ui->lineEdit_6->setReadOnly(true); // Phone Number
    ui->tblMyActivity->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblMyPermissions->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->dateEdit_2->setDate(QDate::currentDate().addDays(-30));
    ui->dateEdit_3->setDate(QDate::currentDate());

    ui->tblPatient->setSelectionBehavior(QAbstractItemView::SelectRows);

    loadProfileData();
    navigateToPage(0, ui->btnDashboard);
    on_btnSearch_4_clicked();

    ui->txtRecordID->setReadOnly(true);
    ui->txtRecordID_2->setReadOnly(true);
    ui->txtRecordID_4->setReadOnly(true);
    ui->txtRecordID_3->setReadOnly(true);
    ui->txtRecordID_5->setReadOnly(true);

    ui->tblRecordPrescription->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblMedicalRecords->setEditTriggers(QAbstractItemView::NoEditTriggers);
    header = ui->tblMedicalRecords->horizontalHeader();

    // medicalrecord table
    header->setSectionResizeMode(0, QHeaderView::Fixed); // id
    header->resizeSection(0, 65);
    header->setSectionResizeMode(1, QHeaderView::Interactive); // patient
    header->resizeSection(1, 160);
    header->setSectionResizeMode(2, QHeaderView::Fixed); // date
    header->resizeSection(2, 110);
    header->setSectionResizeMode(3, QHeaderView::Interactive); // doctor
    header->resizeSection(3, 160);
    header->setSectionResizeMode(4, QHeaderView::Stretch); // Diagnosis
    header->setSectionResizeMode(5, QHeaderView::Fixed); // status
    header->resizeSection(5, 100);
    ui->tblMedicalRecords->setWordWrap(true);
    ui->tblMedicalRecords->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->cbDisease->setEditable(true);
    QStringList diseaseList = Doctor::GetDiseasesList();
    
    // Add only a small subset to the combobox visual items list to avoid GUI lag
    QStringList limitedList;
    for (int i = 0; i < qMin(100, diseaseList.size()); ++i) {
        limitedList.append(diseaseList[i]);
    }
    ui->cbDisease->addItems(limitedList);
 
    // Use the full 70,000+ disease list in the QCompleter with substring search support
    QCompleter *completer = new QCompleter(diseaseList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains); // Matches anywhere in the condition name!
    completer->setCompletionMode(QCompleter::PopupCompletion);
    ui->cbDisease->setCompleter(completer);

    connect(ui->cbDisease, &QComboBox::editTextChanged, this, [=](const QString& text) {
        QString icd = Doctor::GetICDCodeByName(text);
        ui->lineEdit->setText(icd);
    });
}

DoctorWindow::~DoctorWindow()
{
    delete ui;
}

void DoctorWindow::on_btnDashboard_clicked()
{
    navigateToPage(0, ui->btnDashboard);
    on_btnSearch_4_clicked();
}

void DoctorWindow::on_btnPatients_clicked()
{
    navigateToPage(1, ui->btnPatients);
    on_btnSearch_5_clicked();
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

void DoctorWindow::on_btnSearch_5_clicked() // search medical record
{
    QString keyword = ui->txtSearch_5->text();
    QString status = ui->cbStatusFilter->currentText();
    QDate date = ui->dateFilter->date();
    QString dateFilter = "";
    if (date != QDate(1970, 1, 1)) {
        dateFilter = date.toString("yyyy-MM-dd");
    }
    qDebug() << "=== ĐANG CHẠY HÀM SEARCH ===";
    qDebug() << "Keyword:" << keyword << "| Status:" << status << "| Date:" << dateFilter;
    QList<MedicalRecord> records = Doctor::SearchRecordsBy(keyword, status, dateFilter);

    qDebug() << "Số bệnh án tìm thấy trong DB:" << records.size();

    ui->tblMedicalRecords->setRowCount(0);

    for (int i = 0; i < records.size(); ++i) {
        ui->tblMedicalRecords->insertRow(i);
        MedicalRecord rec = records[i];

        ui->tblMedicalRecords->setItem(i, 0, new QTableWidgetItem(QString::number(rec.GetRecordID())));
        ui->tblMedicalRecords->setItem(i, 1, new QTableWidgetItem(rec.GetPatientName()));
        ui->tblMedicalRecords->setItem(i, 2, new QTableWidgetItem(rec.GetDate()));

        QList<QString> doctorsList;
        QList<QString> diagnosesList;

        Doctor::GetAllDiagnosesForRecord(rec, doctorsList, diagnosesList);

        QString doctorsText = doctorsList.join("\n");
        QString diagnosesText = diagnosesList.join("\n");
        ui->tblMedicalRecords->setItem(i, 3, new QTableWidgetItem(doctorsText));
        ui->tblMedicalRecords->setItem(i, 4, new QTableWidgetItem(diagnosesText));

        ui->tblMedicalRecords->resizeRowToContents(i);

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

    QDate visitDate = QDate::fromString(rec.GetDate(), "yyyy-MM-dd");
    ui->dateEdit->setDate(visitDate);
    ui->dateEdit->setDisplayFormat("dd-MM-yyyy");

    QString patientName;
    QList<QString> doctorsList;
    QList<QString> doctorIdsList;
    QList<QString> diagnosesList;

    Doctor::GetRecordExtraInfo(recordId, patientName, doctorsList, doctorIdsList, diagnosesList);

    ui->txtRecordID_2->setText(patientName);
    ui->txtRecordID_3->setText(doctorsList.join("\n"));
    ui->txtRecordID_4->setText(doctorIdsList.join(", "));
    ui->txtRecordID_5->setText(diagnosesList.join("\n"));

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
}

void DoctorWindow::on_btnSearch_4_clicked() {
    QString keyword = ui->txtSearch_4->text();
    currentPatientList = Doctor::SearchPatientBy(keyword);

    // ui->tblPatient->setRowCount(currentPatientList.size());
    ui->tblPatient->setRowCount(currentPatientList.size());


    for (int i = 0; i < currentPatientList.size(); ++i) {
        const Patient& p = currentPatientList[i];
        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(p.ID));
        ui->tblPatient->setItem(i, 0, idItem);

        QTableWidgetItem *nameItem = new QTableWidgetItem(p.FullName);
        ui->tblPatient->setItem(i, 1, nameItem);
        // ui->tblPatient->setItem(i, 2, new QTableWidgetItem(p.Phone));
        // ui->tblPatient->setItem(i, 3, new QTableWidgetItem(p.BirthDate));
        // ui->tblPatient->setItem(i, 4, new QTableWidgetItem(p.Sex));
        // ui->tblPatient->setItem(i, 5, new QTableWidgetItem(p.Address));
    }
    QHeaderView *header = ui->tblPatient->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    header->resizeSection(0, 70);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
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
    QString lastCondition, lastICD, lastSeverity;

    if (Doctor::GetLatestDiagnosis(p.ID, lastCondition, lastICD, lastSeverity)) {
        ui->cbDisease->blockSignals(true);
        ui->cbDisease->setCurrentText(lastCondition);
        ui->cbDisease->blockSignals(false);

        ui->lineEdit->setText(lastICD);
        ui->cbSeverity->setCurrentText(lastSeverity);
    }
    else {
        // Nếu là bệnh nhân mới tinh (chưa có hồ sơ chẩn đoán), làm trắng form
        ui->cbDisease->setCurrentText("");
        ui->lineEdit->clear();
        ui->cbSeverity->setCurrentIndex(0);
    }
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

    // Gắn tính năng Search (QCompleter) - CẬP NHẬT ÉP KIỂU QStringList
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

        // Không lưu vào db ngay mà lưu tạm trên UI
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