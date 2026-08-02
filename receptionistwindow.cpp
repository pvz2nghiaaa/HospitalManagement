#include "receptionistwindow.h"
#include "ui_ReceptionistWindow.h"
#include "user.h"
#include "drug.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QDebug>
#include "loginwindow.h"
#include "patient.h"
#include "doctor.h"
#include "medicalrecord.h"
#include "diagnosis.h"
#include <QDate>
#include <QRandomGenerator>
ReceptionistWindow::ReceptionistWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::ReceptionistWindow)
{
    ui->setupUi(this);

    // default hide overlay
    ui->overlayPatientFrame->hide();
    ui->overlayEditPatientFrame->hide();

    ui->lblAdmin->setText("Rect. " + User::GetActiveUser().GetFullName()+ " (Online)");
    navigateToPage(0, ui->btnPatient);
    ReceptionistWindow::fetchPatient();



    // Drug Management
    ui->tblPatient_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblPatient_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblPatient_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblPatient_2->horizontalHeader()->setStretchLastSection(true);
    // Load all drugs
    loadAllDrugs();

    // Available Doctors Table setup
    ui->tblAvaiDoctor->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblAvaiDoctor->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblAvaiDoctor->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblAvaiDoctor->horizontalHeader()->setStretchLastSection(true);
    ui->tblAvaiDoctor->verticalHeader()->setVisible(false);
}

ReceptionistWindow::~ReceptionistWindow()
{
    delete ui;
}

void ReceptionistWindow::on_btnPatient_clicked()
{
    navigateToPage(0, ui->btnPatient);
}


void ReceptionistWindow::on_btnRecord_clicked()
{
    navigateToPage(1, ui->btnRecord);
    ui->dateEdit->setDate(QDate::currentDate());
    loadAvailableDoctorsToTable();
}


void ReceptionistWindow::on_btnInvoice_clicked()
{
    navigateToPage(2, ui->btnInvoice);
}


void ReceptionistWindow::on_btnDrug_clicked()
{
    navigateToPage(3, ui->btnDrug);
}


void ReceptionistWindow::on_btnProfile_clicked()
{
    navigateToPage(4, ui->btnProfile);
}

void ReceptionistWindow::navigateToPage(int pageIndex, QPushButton* activeBtn) {
    ui->stackedWidget->setCurrentIndex(pageIndex);
    QList<QPushButton*> sidebarButtons = {
        ui->btnPatient,
        ui->btnRecord,
        ui->btnInvoice,
        ui->btnDrug,
        ui->btnProfile
    };
    for (QPushButton* btn : sidebarButtons) {
        btn->setProperty("active", false); // "active" properties along with its style is declared in .ui
        btn->style()->unpolish(btn); // refresh stylesheet
        btn->style()->polish(btn);
    }

    activeBtn->setProperty("active", true);
    activeBtn->style()->unpolish(activeBtn);
    activeBtn->style()->polish(activeBtn);

}

// Drug Management


void ReceptionistWindow::on_btnSearch_14_clicked()
{
    QString keyword = ui->txtSearch_8->text().trimmed();

    QString stockStatus = ui->comboBox_3->currentText();

    QList<Drug> drugs = Drug::SearchDrugsBy(keyword, stockStatus);

    ui->tblPatient_2->setRowCount(0);

    for (int i = 0; i < drugs.size(); i++)
    {
        ui->tblPatient_2->insertRow(i);

        ui->tblPatient_2->setItem(i, 0,
            new QTableWidgetItem(QString::number(drugs[i].getDrugID())));

        ui->tblPatient_2->setItem(i, 1,
            new QTableWidgetItem(drugs[i].getName()));

        ui->tblPatient_2->setItem(i, 2,
            new QTableWidgetItem(drugs[i].getUnit()));

        ui->tblPatient_2->setItem(i, 3,
            new QTableWidgetItem(QString::number(drugs[i].getPrice(), 'f', 2)));

        ui->tblPatient_2->setItem(i, 4,
            new QTableWidgetItem(QString::number(drugs[i].getStockQuantity())));

        ui->tblPatient_2->setItem(i, 5,
            new QTableWidgetItem(
                Drug::GetDrugStatus(drugs[i].getStockQuantity())));
    }
}

//Refresh
void ReceptionistWindow::loadAllDrugs()
{
    QList<Drug> drugs = Drug::SearchDrugsBy("", "");

    ui->tblPatient_2->setRowCount(0);

    for (int i = 0; i < drugs.size(); i++)
    {
        ui->tblPatient_2->insertRow(i);

        ui->tblPatient_2->setItem(i, 0,
            new QTableWidgetItem(QString::number(drugs[i].getDrugID())));

        ui->tblPatient_2->setItem(i, 1,
            new QTableWidgetItem(drugs[i].getName()));

        ui->tblPatient_2->setItem(i, 2,
            new QTableWidgetItem(drugs[i].getUnit()));

        ui->tblPatient_2->setItem(i, 3,
            new QTableWidgetItem(
                QString::number(drugs[i].getPrice(), 'f', 2)));

        ui->tblPatient_2->setItem(i, 4,
            new QTableWidgetItem(
                QString::number(drugs[i].getStockQuantity())));

        ui->tblPatient_2->setItem(i, 5,
            new QTableWidgetItem(
                Drug::GetDrugStatus(drugs[i].getStockQuantity())));
    }
}

void ReceptionistWindow::on_btnSearch_18_clicked()
{
    ui->txtSearch_8->clear();
    ui->comboBox_3->setCurrentIndex(0);
    loadAllDrugs();
}





void ReceptionistWindow::on_btnSearch_15_clicked()
{
    bool ok;

    // Drug Name
    QString name = QInputDialog::getText(this, "Add New Drug", "Drug Name:", QLineEdit::Normal, "", &ok);
    if (!ok)
        return;
    name = name.trimmed();
    if (name.isEmpty())
    {
        QMessageBox::warning(this, "Invalid Input", "Drug name cannot be empty."
        );
        return;
    }

    // Unit
    QString unit =
        QInputDialog::getText(this, "Add New Drug", "Unit:", QLineEdit::Normal, "", &ok);
    if (!ok)
        return;
    unit = unit.trimmed();

    // Price
    double price = QInputDialog::getDouble(this, "Add New Drug", "Price:", 0.0, 0.0, 1000000000.0, 2, &ok);
    if (!ok)
        return;

    // Stock Quantity
    int stock = QInputDialog::getInt(this, "Add New Drug", "Stock Quantity:", 0, 0, 1000000, 1, &ok);
    if (!ok)
        return;


    // Add to database=
    if (Drug::AddNewDrug(name, unit, price, stock)) {
        QMessageBox::information(this, "Success", "New drug added successfully.");

        loadAllDrugs();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Failed to add new drug."
        );
    }
}




void ReceptionistWindow::on_btnSearch_16_clicked()
{
    int row = ui->tblPatient_2->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(this, "Warning", "Please select a drug first."
        );
        return;
    }


    // Get current drug
    int drugID =
        ui->tblPatient_2
        ->item(row, 0)
        ->text()
        .toInt();

    QString oldName =
        ui->tblPatient_2
        ->item(row, 1)
        ->text();

    QString oldUnit =
        ui->tblPatient_2
        ->item(row, 2)
        ->text();

    double oldPrice =
        ui->tblPatient_2
        ->item(row, 3)
        ->text()
        .toDouble();

    int oldStock =
        ui->tblPatient_2
        ->item(row, 4)
        ->text()
        .toInt();


    bool ok;


    // New Drug Name
    QString name = QInputDialog::getText(this, "Update Drug", "Drug Name:", QLineEdit::Normal, oldName, &ok);
    if (!ok)
        return;


    // New Unit
    QString unit = QInputDialog::getText(this, "Update Drug", "Unit:", QLineEdit::Normal, oldUnit, &ok);
    if (!ok)
        return;


    // New Price
    double price = QInputDialog::getDouble(this, "Update Drug", "Price:", oldPrice, 0.0, 1000000000.0, 2, &ok);
    if (!ok)
        return;


    // New Stock
    int stock = QInputDialog::getInt(this, "Update Drug", "Stock Quantity:", oldStock, 0, 1000000, 1, &ok);
    if (!ok)
        return;


    // Update database
    if (Drug::UpdateDrugInfo(drugID, name, unit, price, stock))
    {
        QMessageBox::information(this, "Success", "Drug information updated successfully.");

        loadAllDrugs();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Failed to update drug."
        );
    }
}

void ReceptionistWindow::on_tblPatient_2_cellClicked(int row, int column) {
    Q_UNUSED(column);

    if (row < 0)
        return;

    int drugID =
        ui->tblPatient_2
        ->item(row, 0)
        ->text()
        .toInt();

    QString drugName =
        ui->tblPatient_2
        ->item(row, 1)
        ->text();

    qDebug()
        << "Selected Drug:"
        << drugID
        << drugName;
}



void ReceptionistWindow::on_btnSearch_17_clicked()
{

    int row =
        ui->tblPatient_2->currentRow();


    if (row < 0)
    {
        QMessageBox::warning(
            this,
            "Delete",
            "Select a drug first"
        );
        return;
    }



    int drugID =
        ui->tblPatient_2
        ->item(row, 0)
        ->text()
        .toInt();



    if (Drug::RemoveDrug(drugID))
    {

        QMessageBox::information(
            this,
            "Delete",
            "Delete successfully"
        );
    }
}


void ReceptionistWindow::on_btnLogout_clicked()
{
    User::logout();
    QMessageBox::information(this, "Program info", "Logged out successfully!");

    LoginWindow* loginWin = new LoginWindow();
    loginWin->setAttribute(Qt::WA_DeleteOnClose);
    loginWin->show();

    this->close();
}


void ReceptionistWindow::setBackgroundActiveState(const bool activeState)
{
    ui->stackedWidget->setEnabled(activeState);
    ui->btnLogout->setEnabled(activeState);

    for (int i = 0; i < ui->verticalLayout->count(); i++)
        ui->verticalLayout->itemAt(i)->widget()->setEnabled(activeState);
}

void ReceptionistWindow::showOverlayPatientFrame()
{
    // disable background
    ReceptionistWindow::setBackgroundActiveState(false);

    // set card position
    ui->overlayPatientFrame->setGeometry(0, 0, this->width(), this->height());
    int cardX = (this->width() - ui->cardRegisterPatient->width()) / 2;
    int cardY = (this->height() - ui->cardRegisterPatient->height()) / 2;
    ui->cardRegisterPatient->move(cardX, cardY);

    ui->overlayPatientFrame->show();
    ui->overlayPatientFrame->raise();
}

void ReceptionistWindow::hideOverlayPatientFrame()
{
    ui->overlayPatientFrame->hide();
    ReceptionistWindow::setBackgroundActiveState(true);
}

void ReceptionistWindow::showOverlayEditPatientFrame()
{
    // disable background
    ReceptionistWindow::setBackgroundActiveState(false);

    // set card position
    ui->overlayEditPatientFrame->setGeometry(0, 0, this->width(), this->height());
    int cardX = (this->width() - ui->cardEditPatient->width()) / 2;
    int cardY = (this->height() - ui->cardEditPatient->height()) / 2;
    ui->cardEditPatient->move(cardX, cardY);

    ui->overlayEditPatientFrame->show();
    ui->overlayEditPatientFrame->raise();
}

void ReceptionistWindow::hideOverlayEditPatientFrame()
{
    ui->overlayEditPatientFrame->hide();
    ReceptionistWindow::setBackgroundActiveState(true);
}

void ReceptionistWindow::on_btnNewPatient_clicked()
{
    ReceptionistWindow::showOverlayPatientFrame();
}

void ReceptionistWindow::on_btnCancelPat_clicked()
{
    ReceptionistWindow::hideOverlayPatientFrame();
    ReceptionistWindow::fetchPatient();
}


void ReceptionistWindow::on_btnSavePat_clicked()
{
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

        // Reset fields
        ui->txtPatFullName->clear();
        ui->txtPatPhone->clear();
        ui->datePatDOB->setDate(QDate::currentDate());
        ui->cbPatGender->setCurrentIndex(0);
        ui->txtPatAddress->clear();

        ReceptionistWindow::hideOverlayPatientFrame();
    } else {
        QMessageBox::critical(this, "Error", "Failed to register patient in database.");
    }
    ReceptionistWindow::fetchPatient();
}

void ReceptionistWindow::fetchPatient()
{
    QString searchID = ui->patientIDLineEdit->text().trimmed();
    QString searchName = ui->patientNameLineEdit->text().trimmed();
    QString searchPhone = ui->patientPhoneLineEdit->text().trimmed();

    QList<std::tuple<QString, QString, QString, QString, QString, QString>> lstPatient = Patient::findPatient(searchID, searchName, searchPhone);

    ui->tblPatient->setRowCount(0);

    int row = 0;
    for (; row < (int)lstPatient.size(); row++) {
        ui->tblPatient->insertRow(row);

        ui->tblPatient->setItem(row, 0, new QTableWidgetItem(get<0>(lstPatient[row])));
        ui->tblPatient->setItem(row, 1, new QTableWidgetItem(get<1>(lstPatient[row])));
        ui->tblPatient->setItem(row, 2, new QTableWidgetItem(get<2>(lstPatient[row])));
        ui->tblPatient->setItem(row, 3, new QTableWidgetItem(get<3>(lstPatient[row])));
        ui->tblPatient->setItem(row, 4, new QTableWidgetItem(get<4>(lstPatient[row])));
        ui->tblPatient->setItem(row, 5, new QTableWidgetItem(get<5>(lstPatient[row])));
    }

    // Optional feedback if zero rows matched
    if (row == 0) {
        QMessageBox::information(this, "No Results", "No matching records found.");
    }
}

void ReceptionistWindow::on_btnPatientSearch_clicked()
{
    ReceptionistWindow::fetchPatient();
}

void ReceptionistWindow::on_btnRefreshPatient_clicked()
{
    ReceptionistWindow::fetchPatient();
}

void ReceptionistWindow::on_tblPatient_cellDoubleClicked(int row, int column)
{
    ReceptionistWindow::showOverlayEditPatientFrame();
    ui->lblEditPatTitle->setText(QString("Edit Patient #" + ui->tblPatient->item(row, 0)->text()));
    ui->txtEditPatFullName->setText(ui->tblPatient->item(row, 1)->text());
    ui->txtEditPatPhone->setText(ui->tblPatient->item(row, 2)->text());
    ui->dateEditPatDOB->setDate(QDate::fromString(ui->tblPatient->item(row, 3)->text(), "dd-MM-yyyy"));
    ui->cbEditPatGender->setCurrentText(ui->tblPatient->item(row, 4)->text());
    ui->txtEditPatAddress->setText(ui->tblPatient->item(row, 5)->text());
}

void ReceptionistWindow::on_btnCancelEditPat_clicked()
{
    ReceptionistWindow::hideOverlayEditPatientFrame();
    ReceptionistWindow::fetchPatient();
}


void ReceptionistWindow::on_btnSaveEditPat_clicked()
{
    QString id = ui->lblEditPatTitle->text().remove("Edit Patient #").trimmed();
    QString name = ui->txtEditPatFullName->text().trimmed();
    QString phone = ui->txtEditPatPhone->text().trimmed();
    QString dob = ui->dateEditPatDOB->date().toString("yyyy-MM-dd");
    QString sex = ui->cbEditPatGender->currentText();
    QString addr = ui->txtEditPatAddress->text().trimmed();

    if (Patient::updatePatient(id, name, phone, dob, sex, addr))
    {
        QMessageBox::information(this, "Success", "Patient info updated successfully!");

        // Reset fields
        ui->txtEditPatFullName->clear();
        ui->txtEditPatPhone->clear();
        ui->dateEditPatDOB->setDate(QDate::currentDate());
        ui->cbEditPatGender->setCurrentIndex(0);
        ui->txtEditPatAddress->clear();

        ReceptionistWindow::hideOverlayEditPatientFrame();
    } else {
        QMessageBox::critical(this, "Error", "Failed to edit patient in database.");
    }
    ReceptionistWindow::fetchPatient();
}

// FrameMedicalRecord UI Implementation
void ReceptionistWindow::loadAvailableDoctorsToTable()
{
    QList<std::pair<int, QString>> doctors = Doctor::getAvailableDoctors();
    ui->tblAvaiDoctor->setRowCount(0);
    for (int i = 0; i < doctors.size(); i++) {
        ui->tblAvaiDoctor->insertRow(i);
        ui->tblAvaiDoctor->setItem(i, 0, new QTableWidgetItem(QString::number(doctors[i].first)));
        ui->tblAvaiDoctor->setItem(i, 1, new QTableWidgetItem(doctors[i].second));
    }
}

void ReceptionistWindow::on_txtRecordID_textChanged(const QString &text)
{
    bool ok = false;
    int patientId = text.trimmed().toInt(&ok);
    if (!ok || patientId <= 0) {
        ui->txtRecordID_2->clear();
        ui->txtRecordID_5->clear();
        return;
    }

    Patient p;
    if (Patient::getPatientById(patientId, p)) {
        ui->txtRecordID_2->setText(p.FullName);
        ui->txtRecordID_5->setText(p.Phone);

        int sexIndex = ui->comboBox->findText(p.Sex, Qt::MatchExactly);
        if (sexIndex >= 0) {
            ui->comboBox->setCurrentIndex(sexIndex);
        } else {
            ui->comboBox->setCurrentText(p.Sex);
        }
    } else {
        ui->txtRecordID_2->clear();
        ui->txtRecordID_5->clear();
    }
}

void ReceptionistWindow::on_tblAvaiDoctor_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    if (row < 0 || row >= ui->tblAvaiDoctor->rowCount()) return;

    QTableWidgetItem *idItem = ui->tblAvaiDoctor->item(row, 0);
    QTableWidgetItem *nameItem = ui->tblAvaiDoctor->item(row, 1);

    if (idItem && nameItem) {
        ui->txtRecordID_4->setText(idItem->text());
        ui->txtRecordID_3->setText(nameItem->text());
    }
}

void ReceptionistWindow::on_btnCreateRec_clicked()
{
    bool okPat = false;
    int patientId = ui->txtRecordID->text().trimmed().toInt(&okPat);
    Patient patient;
    if (!okPat || !Patient::getPatientById(patientId, patient)) {
        QMessageBox::warning(this, "Validation Error", "Please enter a valid existing Patient ID.");
        return;
    }

    bool okDoc = false;
    int doctorId = ui->txtRecordID_4->text().trimmed().toInt(&okDoc);
    if (!okDoc || !Doctor::isDoctorAvailable(doctorId)) {
        QMessageBox::warning(this, "Validation Error", "Please select an assigned doctor who is currently available.");
        return;
    }

    QString recordDate = ui->dateEdit->date().toString("yyyy-MM-dd");
    int recordId = MedicalRecord::createRecord(patientId, recordDate);
    if (recordId <= 0) {
        QMessageBox::critical(this, "Database Error", "Failed to create medical record.");
        return;
    }

    bool diagCreated = Diagnosis::createDiagnosis(doctorId, recordId, patient.FullName);
    if (!diagCreated) {
        QMessageBox::critical(this, "Database Error", "Medical record created, but failed to create initial diagnosis.");
        return;
    }

    QMessageBox::information(this, "Success", QString("Medical Record #%1 created successfully with Diagnosis for patient '%2'!")
                                                  .arg(recordId).arg(patient.FullName));
}

void ReceptionistWindow::on_btnAutoCreateRec_clicked()
{
    bool okPat = false;
    int patientId = ui->txtRecordID->text().trimmed().toInt(&okPat);
    Patient patient;
    if (!okPat || !Patient::getPatientById(patientId, patient)) {
        QMessageBox::warning(this, "Validation Error", "Please enter a valid existing Patient ID before auto creating a record.");
        return;
    }

    QList<std::pair<int, QString>> availableDoctors = Doctor::getAvailableDoctors();
    if (availableDoctors.isEmpty()) {
        QMessageBox::warning(this, "Assignment Error", "No available doctors found in the system.");
        return;
    }

    int randomIndex = QRandomGenerator::global()->bounded(availableDoctors.size());
    auto selectedDoctor = availableDoctors[randomIndex];

    ui->txtRecordID_4->setText(QString::number(selectedDoctor.first));
    ui->txtRecordID_3->setText(selectedDoctor.second);

    QString recordDate = ui->dateEdit->date().toString("yyyy-MM-dd");
    int recordId = MedicalRecord::createRecord(patientId, recordDate);
    if (recordId <= 0) {
        QMessageBox::critical(this, "Database Error", "Failed to create medical record.");
        return;
    }

    bool diagCreated = Diagnosis::createDiagnosis(selectedDoctor.first, recordId, patient.FullName);
    if (!diagCreated) {
        QMessageBox::critical(this, "Database Error", "Medical record created, but failed to create initial diagnosis.");
        return;
    }

    QMessageBox::information(this, "Success", QString("Auto created Medical Record #%1! Assigned to Dr. %2 for patient '%3'.")
                                                  .arg(recordId).arg(selectedDoctor.second).arg(patient.FullName));
}

void ReceptionistWindow::on_btnClear_clicked()
{
    ui->txtRecordID->clear();
    ui->txtRecordID_2->clear();
    ui->txtRecordID_3->clear();
    ui->txtRecordID_4->clear();
    ui->txtRecordID_5->clear();
}


