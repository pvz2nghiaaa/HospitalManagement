#include "receptionistwindow.h"
#include "billingmanager.h"
#include "receptionist.h"
#include "permission.h"
#include "attendancelog.h"
#include "ui_ReceptionistWindow.h"
#include "user.h"
#include "drug.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QDebug>
#include <QMap>
#include <QDate>
#include <QDateTime>
#include <QTableWidgetItem>
#include "loginwindow.h"
#include "patient.h"
#include "attendancelog.h"
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
    ui->overlayDrugFrame->hide();
    ui->cardAddDrug->hide();
    ui->cardEditDrug->hide();
    ui->cardDrugHistory->hide();
    ui->overlayEditPatientFrame->hide();

    ui->lblAdmin->setText("Rect. " + User::GetActiveUser().GetFullName()+ " (Online)");
    navigateToPage(0, ui->btnPatient);
    ReceptionistWindow::fetchPatient();
    //Invoice
    // Danh sách invoice
    ui->tableWidget->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    ui->tableWidget->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    ui->tableWidget->setSelectionMode(
        QAbstractItemView::SingleSelection
    );

    ui->tableWidget->verticalHeader()->hide();

    // Bảng item của invoice
    ui->tableWidget_2->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    ui->tableWidget_2->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    ui->tableWidget_2->verticalHeader()->hide();

    ui->tableWidget_2
        ->horizontalHeader()
        ->setSectionResizeMode(
            QHeaderView::Stretch
        );

    // Status chỉ để hiển thị
    ui->radioButton->setEnabled(false);
    ui->radioButton_2->setEnabled(false);

    // Các ô chi tiết không cho sửa
    ui->txtRecordID_7->setReadOnly(true);
    ui->txtRecordID_8->setReadOnly(true);
    ui->txtRecordID_10->setReadOnly(true);
    ui->txtRecordID_9->setReadOnly(true);

    clearInvoiceDetails();


    // Drug Management
    ui->tblPatient_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblPatient_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblPatient_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblPatient_2->horizontalHeader()->setStretchLastSection(true);
    // Load all drugs
    loadAllDrugs();


    // Attendance Table setup
    ui->tblAttendance->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblAttendance->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblAttendance->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblAttendance->horizontalHeader()->setStretchLastSection(true);
    ui->tblAttendance->verticalHeader()->setVisible(false);
    ui->tblAttendance->setColumnWidth(0, 100);
    ui->tblAttendance->setColumnWidth(1, 300);
    ui->tblAttendance->setColumnWidth(2, 200);


    //My profile
    ui->dateFromProfile->setDisplayFormat("dd-MM-yyyy");
    ui->dateToProfile->setDisplayFormat("dd-MM-yyyy");

    ui->dateFromProfile->setCalendarPopup(true);
    ui->dateToProfile->setCalendarPopup(true);

    ui->dateToProfile->setDate(QDate::currentDate());
    ui->dateFromProfile->setDate(QDate::currentDate().addMonths(-1));
    ui->tblMyPermissions->setFocusPolicy(Qt::NoFocus);
    ui->tblMyPermissions->setSelectionMode(
        QAbstractItemView::NoSelection
    );

    ui->tblMyActivityLogs->setFocusPolicy(Qt::NoFocus);
    ui->tblMyActivityLogs->setSelectionMode(
        QAbstractItemView::NoSelection
    );
    ui->tblMyActivityLogs->setColumnCount(2);

    ui->tblMyActivityLogs->setHorizontalHeaderLabels(
        QStringList() << "Date" << "Status"
    );

    ui->tblMyActivityLogs->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    ui->tblMyActivityLogs->verticalHeader()->setVisible(false);

    ui->tblMyActivityLogs->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    ui->tblMyActivityLogs->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

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
    ui->tabWidget_2->setCurrentIndex(0);

    ui->dateFromProfile->setDate(
        QDate::currentDate().addMonths(-1)
    );

    ui->dateToProfile->setDate(
        QDate::currentDate()
    );

    loadMyProfileInfo();
    loadMyPermissions();
    loadMyActivityLogs();
}

void ReceptionistWindow::navigateToPage(int pageIndex, QPushButton* activeBtn) {
    ui->stackedWidget->setCurrentIndex(pageIndex);
    QList<QPushButton*> sidebarButtons = {
        ui->btnPatient,
        ui->btnRecord,
        ui->btnInvoice,
        ui->btnDrug,
        ui->btnAttendance,
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
    showAddDrugFrame();
}




void ReceptionistWindow::on_btnSearch_16_clicked()
{
    int row = ui->tblPatient_2->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(
            this,
            "Warning",
            "Please select a drug first."
        );
        return;
    }

    currentDrugID =
        ui->tblPatient_2
        ->item(row, 0)
        ->text()
        .toInt();

    ui->txtEditDrugName->setText(
        ui->tblPatient_2->item(row, 1)->text());

    ui->txtEditDrugUnit->setText(
        ui->tblPatient_2->item(row, 2)->text());

    ui->spEditDrugPrice->setValue(
        ui->tblPatient_2->item(row, 3)->text().toDouble());

    ui->spEditDrugStock->setValue(
        ui->tblPatient_2->item(row, 4)->text().toInt());

    showEditDrugFrame();
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

        QMessageBox::information(this,"Delete","Delete successfully");
        loadAllDrugs();
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


void ReceptionistWindow::on_btnAttendance_clicked()
{
    navigateToPage(5, ui->btnAttendance);
    ui->dateAttendance->setDate(QDate::currentDate());
    refreshAttendanceTable();
}

void ReceptionistWindow::on_btnSearchAttendance_clicked()
{
    refreshAttendanceTable();
}

void ReceptionistWindow::on_btnMarkPresent_clicked()
{
    int row = ui->tblAttendance->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Selection Required", "Please select a staff member from the table.");
        return;
    }
    
    int userId = ui->tblAttendance->item(row, 0)->text().toInt();
    QString selectedDate = ui->dateAttendance->date().toString("dd-MM-yyyy");
    
    AttendanceLog* existingLog = AttendanceLog::GetLogByEmployeeAndDate(userId, selectedDate);
    int logId = existingLog ? existingLog->getId() : -1;
    if (existingLog) {
        delete existingLog;
    }
    
    AttendanceLog log(logId, selectedDate, 1, userId);
    if (log.save()) {
        refreshAttendanceTable();
        ui->tblAttendance->setCurrentCell(row, 0);
    } else {
        QMessageBox::critical(this, "Error", "Failed to mark attendance.");
    }
}

void ReceptionistWindow::on_btnMarkAbsent_clicked()
{
    int row = ui->tblAttendance->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Selection Required", "Please select a staff member from the table.");
        return;
    }
    
    int userId = ui->tblAttendance->item(row, 0)->text().toInt();
    QString selectedDate = ui->dateAttendance->date().toString("dd-MM-yyyy");
    
    AttendanceLog* existingLog = AttendanceLog::GetLogByEmployeeAndDate(userId, selectedDate);
    int logId = existingLog ? existingLog->getId() : -1;
    if (existingLog) {
        delete existingLog;
    }
    
    AttendanceLog log(logId, selectedDate, 0, userId);
    if (log.save()) {
        refreshAttendanceTable();
        ui->tblAttendance->setCurrentCell(row, 0);
    } else {
        QMessageBox::critical(this, "Error", "Failed to mark attendance.");
    }
}

void ReceptionistWindow::refreshAttendanceTable() {
    ui->tblAttendance->setRowCount(0);
    
    QString selectedDate = ui->dateAttendance->date().toString("dd-MM-yyyy");
    
    auto users = User::GetAllUser();
    QList<AttendanceLog> logs = AttendanceLog::SearchByDate(selectedDate);
    
    QMap<int, AttendanceLog> logMap;
    for (const AttendanceLog &log : logs) {
        logMap[log.getEmployeeId()] = log;
    }
    
    int row = 0;
    for (const auto& u : users) {
        int userId = get<0>(u);
        QString fullName = get<2>(u);
        QString role = get<5>(u);
        
        ui->tblAttendance->insertRow(row);
        ui->tblAttendance->setItem(row, 0, new QTableWidgetItem(QString::number(userId)));
        ui->tblAttendance->setItem(row, 1, new QTableWidgetItem(fullName));
        ui->tblAttendance->setItem(row, 2, new QTableWidgetItem(role));
        
        QString status = "Not Marked";
        if (logMap.contains(userId)) {
            status = (logMap[userId].getIsPresent() == 1) ? "Present" : "Absent";
        }
        
        QTableWidgetItem *statusItem = new QTableWidgetItem(status);
        if (status == "Present") {
            statusItem->setForeground(QBrush(QColor(34, 197, 94))); // green
        } else if (status == "Absent") {
            statusItem->setForeground(QBrush(QColor(239, 68, 68))); // red
        } else {
            statusItem->setForeground(QBrush(QColor(148, 163, 184))); // slate/gray
        }
        ui->tblAttendance->setItem(row, 3, statusItem);
        
        row++;
    }
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

void ReceptionistWindow::showAddDrugFrame()
{
    ui->cardEditDrug->hide();
    ui->cardDrugHistory->hide();

    ui->txtAddDrugName->clear();
    ui->txtAddDrugUnit->clear();
    ui->spAddDrugPrice->setValue(0);
    ui->spAddDrugStock->setValue(0);

    ui->overlayDrugFrame->show();
    ui->cardAddDrug->show();

    ui->txtAddDrugName->setFocus();
}

void ReceptionistWindow::hideAddDrugFrame()
{
    ui->cardAddDrug->hide();
    ui->overlayDrugFrame->hide();
}

void ReceptionistWindow::on_btnCancelAddDrug_clicked()
{
    hideAddDrugFrame();
}


void ReceptionistWindow::on_btnSaveAddDrug_clicked()
{
    QString name = ui->txtAddDrugName->text().trimmed();
    QString unit = ui->txtAddDrugUnit->text().trimmed();
    double price = ui->spAddDrugPrice->value();
    int stock = ui->spAddDrugStock->value();

    if (Drug::AddNewDrug(name, unit, price, stock))
    {
        QMessageBox::information(
            this,
            "Success",
            "Drug added successfully."
        );

        hideAddDrugFrame();
        loadAllDrugs();
    }
}

void ReceptionistWindow::showEditDrugFrame()
{
    ui->cardAddDrug->hide();
    ui->cardDrugHistory->hide();

    ui->overlayDrugFrame->show();
    ui->cardEditDrug->show();

    ui->txtEditDrugName->setFocus();
}

void ReceptionistWindow::hideEditDrugFrame()
{
    ui->cardEditDrug->hide();
    ui->overlayDrugFrame->hide();
}

void ReceptionistWindow::on_btnCancelEditDrug_clicked()
{
    hideEditDrugFrame();
}
void ReceptionistWindow::on_btnSaveEditDrug_clicked()
{
    QString name =
        ui->txtEditDrugName->text().trimmed();

    QString unit =
        ui->txtEditDrugUnit->text().trimmed();

    double price =
        ui->spEditDrugPrice->value();

    int stock =
        ui->spEditDrugStock->value();

    if (Drug::UpdateDrugInfo(
        currentDrugID,
        name,
        unit,
        price,
        stock))
    {
        QMessageBox::information(
            this,
            "Success",
            "Drug updated successfully."
        );

        hideEditDrugFrame();
        loadAllDrugs();
    }
}


void ReceptionistWindow::on_btnCloseDrugHistory_clicked()
{
    hideDrugHistoryFrame();
}

void ReceptionistWindow::hideDrugHistoryFrame()
{
    ui->cardDrugHistory->hide();
    ui->overlayDrugFrame->hide();
}

void ReceptionistWindow::showDrugHistoryFrame()
{
    ui->cardAddDrug->hide();
    ui->cardEditDrug->hide();

    ui->overlayDrugFrame->setGeometry(
        0,
        0,
        width(),
        height()
    );

    int cardX =
        (ui->overlayDrugFrame->width()
            - ui->cardDrugHistory->width()) / 2;

    int cardY =
        (ui->overlayDrugFrame->height()
            - ui->cardDrugHistory->height()) / 2;

    ui->cardDrugHistory->move(cardX, cardY);

    ui->overlayDrugFrame->show();
    ui->cardDrugHistory->show();
}

void ReceptionistWindow::on_pushButton_clicked()
{
    ui->lblPatTitle_5->setText(
        "Drug Stock History"
    );

    showDrugHistoryFrame();

    GetDrugStockHistory();
}

void ReceptionistWindow::GetDrugStockHistory()
{
    if (!Drug::initTable())
    {
        QMessageBox::critical(
            this,
            "History Error",
            "Cannot initialize DrugStockHistory table."
        );

        return;
    }

    QSqlQuery query;

    if (!query.exec(
        "SELECT "
        "HistoryID, "
        "DrugName, "
        "Action, "
        "Amount, "
        "Time "
        "FROM DrugStockHistory "
        "ORDER BY HistoryID DESC"
    ))
    {
        QMessageBox::critical(
            this,
            "History Error",
            query.lastError().text()
        );

        qDebug()
            << "Load History Error:"
            << query.lastError().text();

        return;
    }

    ui->tblDrugHistory->clearContents();
    ui->tblDrugHistory->setRowCount(0);
    ui->tblDrugHistory->setColumnCount(5);

    ui->tblDrugHistory->setHorizontalHeaderLabels(
        QStringList()
        << "ID"
        << "Drug Name"
        << "Action"
        << "Amount"
        << "Time"
    );

    int row = 0;

    while (query.next())
    {
        ui->tblDrugHistory->insertRow(row);

        QString historyID =
            query.value(0).toString();

        QString drugName =
            query.value(1).toString();

        QString action =
            query.value(2).toString();

        int amount =
            query.value(3).toInt();

        QString amountText;

        if (amount > 0)
        {
            amountText =
                "+" + QString::number(amount);
        }
        else
        {
            amountText =
                QString::number(amount);
        }

        QString rawTime =
            query.value(4).toString();

        QDateTime dt =
            QDateTime::fromString(
                rawTime,
                "yyyy-MM-dd HH:mm:ss"
            );

        QString time;

        if (dt.isValid())
        {
            time =
                dt.toString("HH:mm:ss  dd/MM/yyyy");
        }
        else
        {
            time = rawTime;
        }

        // ID
        QTableWidgetItem* itemID =
            new QTableWidgetItem(historyID);

        itemID->setTextAlignment(
            Qt::AlignCenter
        );

        // Drug Name
        QTableWidgetItem* itemName =
            new QTableWidgetItem(drugName);

        itemName->setTextAlignment(
            Qt::AlignLeft | Qt::AlignVCenter
        );

        // Action
        QTableWidgetItem* itemAction =
            new QTableWidgetItem(action);

        itemAction->setTextAlignment(
            Qt::AlignCenter
        );

        // Amount
        QTableWidgetItem* itemAmount =
            new QTableWidgetItem(amountText);

        itemAmount->setTextAlignment(
            Qt::AlignCenter
        );

        // Time
        QTableWidgetItem* itemTime =
            new QTableWidgetItem(time);

        itemTime->setTextAlignment(
            Qt::AlignCenter
        );

        ui->tblDrugHistory->setItem(
            row,
            0,
            itemID
        );

        ui->tblDrugHistory->setItem(
            row,
            1,
            itemName
        );

        ui->tblDrugHistory->setItem(
            row,
            2,
            itemAction
        );

        ui->tblDrugHistory->setItem(
            row,
            3,
            itemAmount
        );

        ui->tblDrugHistory->setItem(
            row,
            4,
            itemTime
        );

        row++;
    }

    QHeaderView* header =
        ui->tblDrugHistory->horizontalHeader();

    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(4, QHeaderView::Stretch);

    qDebug()
        << "History rows loaded:"
        << row;
}

void ReceptionistWindow::loadMyPermissions()
{
    QList<Permission> permissions =
        Receptionist::GetMyPermissions();

    ui->tblMyPermissions->clearContents();
    ui->tblMyPermissions->setRowCount(0);
    ui->tblMyPermissions->setColumnCount(2);

    ui->tblMyPermissions->setHorizontalHeaderLabels(
        QStringList()
        << "Permission"
        << "Status"
    );

    int row = 0;

    for (const Permission& permission : permissions)
    {
        ui->tblMyPermissions->insertRow(row);

        Permission::Type type =
            static_cast<Permission::Type>(
                permission
                );

        QString name =
            Permission::permissionToReadableString(
                type
            );

        QTableWidgetItem* nameItem =
            new QTableWidgetItem(name);

        QTableWidgetItem* statusItem =
            new QTableWidgetItem("Granted");

        nameItem->setTextAlignment(
            Qt::AlignVCenter | Qt::AlignLeft
        );

        statusItem->setTextAlignment(
            Qt::AlignCenter
        );

        ui->tblMyPermissions->setItem(
            row,
            0,
            nameItem
        );

        ui->tblMyPermissions->setItem(
            row,
            1,
            statusItem
        );

        row++;
    }

    ui->tblMyPermissions
        ->horizontalHeader()
        ->setSectionResizeMode(
            QHeaderView::Stretch
        );

    qDebug()
        << "Loaded permissions:"
        << permissions.size();
}

void ReceptionistWindow::loadMyActivityLogs()
{
    QString dateFrom =
        ui->dateFromProfile->date().toString("dd-MM-yyyy");

    QString dateTo =
        ui->dateToProfile->date().toString("dd-MM-yyyy");

    QList<AttendanceLog> logs =
        Receptionist::SearchMyActivityLogs(dateFrom, dateTo);

    ui->tblMyActivityLogs->clearContents();
    ui->tblMyActivityLogs->setRowCount(logs.size());
    ui->tblMyActivityLogs->setColumnCount(2);

    ui->tblMyActivityLogs->setHorizontalHeaderLabels(
        QStringList() << "Date" << "Status"
    );

    for (int i = 0; i < logs.size(); i++)
    {
        QString status;

        if (logs[i].getIsPresent())
            status = "Present";
        else
            status = "Absent";

        QTableWidgetItem* dateItem =
            new QTableWidgetItem(logs[i].getDate());

        QTableWidgetItem* statusItem =
            new QTableWidgetItem(status);

        dateItem->setTextAlignment(Qt::AlignCenter);
        statusItem->setTextAlignment(Qt::AlignCenter);

        ui->tblMyActivityLogs->setItem(i, 0, dateItem);
        ui->tblMyActivityLogs->setItem(i, 1, statusItem);
    }

    ui->tblMyActivityLogs->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    qDebug() << "Loaded receptionist logs:" << logs.size();
}
void ReceptionistWindow::on_btnSearchActivity_clicked()
{
    if (ui->dateFromProfile->date() >
        ui->dateToProfile->date())
    {
        QMessageBox::warning(
            this,
            "Invalid Date",
            "From Date cannot be later than To Date."
        );

        return;
    }

    loadMyActivityLogs();
}

void ReceptionistWindow::loadMyProfileInfo()
{
    User& user = Receptionist::GetMyProfileInfo();

    ui->lineEdit_2->setText(
        QString::number(user.GetID())
    );

    ui->lineEdit_3->setText(
        user.GetFullName()
    );

    //ui->lineEdit_4->setText(
    //    user.GetGender()
    //);

    //ui->lineEdit_5->setText(
    //    user.GetDateOfBirth()
    //);

    ui->lineEdit_6->setText(
        user.GetPhoneNumber()
    );

    ui->lineEdit_2->setReadOnly(true);
    ui->lineEdit_3->setReadOnly(true);
    ui->lineEdit_4->setReadOnly(true);
    ui->lineEdit_5->setReadOnly(true);
    ui->lineEdit_6->setReadOnly(true);
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


int ReceptionistWindow::getSelectedInvoiceID() const
{
    int row =
        ui->tableWidget->currentRow();

    if (row < 0)
        return -1;

    QTableWidgetItem* item =
        ui->tableWidget->item(row, 0);

    if (item == nullptr)
        return -1;

    bool ok = false;

    int invoiceID =
        item->text().toInt(&ok);

    if (!ok)
        return -1;

    return invoiceID;
}

void ReceptionistWindow::clearInvoiceDetails()
{
    ui->txtRecordID_7->clear();
    ui->txtRecordID_8->clear();
    ui->txtRecordID_10->clear();

    ui->dateEdit_4->clear();

    ui->txtRecordID_9->setText(
        "0.00 VND"
    );

    ui->radioButton->setChecked(false);
    ui->radioButton_2->setChecked(false);

    ui->tableWidget_2->clearContents();
    ui->tableWidget_2->setRowCount(0);
}

void ReceptionistWindow::loadInvoiceDetails(
    int invoiceID)
{
    InvoiceDetails details =
        BillingManager::GetInvoiceDetails(
            invoiceID
        );

    if (!details.found)
    {
        QMessageBox::warning(
            this,
            "Invoice",
            "Invoice details could not be found."
        );

        clearInvoiceDetails();
        return;
    }

    // Invoice ID
    ui->txtRecordID_7->setText(
        QString::number(
            details.invoiceID
        )
    );

    // Patient
    ui->txtRecordID_8->setText(
        details.patientName
    );

    // Doctor
    ui->txtRecordID_10->setText(
        details.doctorName
    );

    // Date
    QDateTime dateTime =
        QDateTime::fromString(
            details.dateIssued,
            "dd-MM-yyyy HH:mm:ss"
        );

    if (dateTime.isValid())
    {
        ui->dateEdit_4->setDate(
            dateTime.date()
        );
    }
    else
    {
        QDate date =
            QDate::fromString(
                details.dateIssued,
                "yyyy-MM-dd"
            );

        if (date.isValid())
        {
            ui->dateEdit_4->setDate(date);
        }
    }

    ui->dateEdit_4->setDisplayFormat(
        "dd/MM/yyyy"
    );

    // Total
    ui->txtRecordID_9->setText(
        QString::number(
            details.totalAmount,
            'f',
            2
        )
        + " VND"
    );

    // Status
    if (details.isPaid)
    {
        ui->radioButton->setChecked(true);
        ui->radioButton_2->setChecked(false);

        // Không cho thanh toán lần nữa
        ui->btnSearch_12->setEnabled(false);
        ui->btnSearch_12->setText("Paid");
    }
    else
    {
        ui->radioButton->setChecked(false);
        ui->radioButton_2->setChecked(true);

        ui->btnSearch_12->setEnabled(true);
        ui->btnSearch_12->setText(
            "💳 Mark as Paid"
        );
    }

    // Danh sách item
    ui->tableWidget_2->clearContents();
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_2->setColumnCount(4);

    ui->tableWidget_2
        ->setHorizontalHeaderLabels(
            QStringList()
            << "Drug / Service"
            << "Quantity"
            << "Price"
            << "Amount"
        );

    int row = 0;

    for (const InvoiceItemDetails& item :
        details.items)
    {
        ui->tableWidget_2->insertRow(row);

        QTableWidgetItem* nameItem =
            new QTableWidgetItem(item.name);

        QTableWidgetItem* quantityItem =
            new QTableWidgetItem(
                QString::number(item.quantity)
            );

        QTableWidgetItem* priceItem =
            new QTableWidgetItem(
                QString::number(
                    item.price,
                    'f',
                    2
                )
            );

        QTableWidgetItem* amountItem =
            new QTableWidgetItem(
                QString::number(
                    item.amount,
                    'f',
                    2
                )
            );

        nameItem->setTextAlignment(
            Qt::AlignLeft |
            Qt::AlignVCenter
        );

        quantityItem->setTextAlignment(
            Qt::AlignCenter
        );

        priceItem->setTextAlignment(
            Qt::AlignRight |
            Qt::AlignVCenter
        );

        amountItem->setTextAlignment(
            Qt::AlignRight |
            Qt::AlignVCenter
        );

        ui->tableWidget_2->setItem(
            row,
            0,
            nameItem
        );

        ui->tableWidget_2->setItem(
            row,
            1,
            quantityItem
        );

        ui->tableWidget_2->setItem(
            row,
            2,
            priceItem
        );

        ui->tableWidget_2->setItem(
            row,
            3,
            amountItem
        );

        row++;
    }

    ui->tableWidget_2
        ->horizontalHeader()
        ->setSectionResizeMode(
            QHeaderView::Stretch
        );
}

void ReceptionistWindow::on_tableWidget_cellClicked(
    int row,
    int column)
{
    Q_UNUSED(column);

    if (row < 0)
        return;

    QTableWidgetItem* invoiceItem =
        ui->tableWidget->item(row, 0);

    if (invoiceItem == nullptr)
        return;

    bool ok = false;

    int invoiceID =
        invoiceItem->text().toInt(&ok);

    if (!ok || invoiceID <= 0)
        return;

    loadInvoiceDetails(invoiceID);
}

void ReceptionistWindow::on_btnSearch_12_clicked()
{
    int invoiceID =
        getSelectedInvoiceID();

    if (invoiceID <= 0)
    {
        QMessageBox::warning(
            this,
            "Payment",
            "Please select an invoice first."
        );

        return;
    }

    InvoiceDetails details =
        BillingManager::GetInvoiceDetails(
            invoiceID
        );

    if (!details.found)
    {
        QMessageBox::warning(
            this,
            "Payment",
            "Invoice could not be found."
        );

        return;
    }

    if (details.isPaid)
    {
        QMessageBox::information(
            this,
            "Payment",
            "This invoice is already paid."
        );

        return;
    }

    QMessageBox::StandardButton answer =
        QMessageBox::question(
            this,
            "Confirm Payment",
            "Are you sure you want to mark "
            "this invoice as paid?",
            QMessageBox::Yes |
            QMessageBox::No
        );

    if (answer != QMessageBox::Yes)
        return;

    if (BillingManager::MarkInvoiceAsPaid(
        invoiceID,
        QString()))
    {
        QMessageBox::information(
            this,
            "Payment",
            "Invoice marked as paid successfully."
        );

        loadInvoiceDetails(invoiceID);

        // loadInvoices();
    }
    else
    {
        QMessageBox::critical(
            this,
            "Payment",
            "Failed to mark invoice as paid."
        );
    }
}

void ReceptionistWindow::on_btnSearch_13_clicked()
{
    int invoiceID =
        getSelectedInvoiceID();

    if (invoiceID <= 0)
    {
        QMessageBox::warning(
            this,
            "Print Invoice",
            "Please select an invoice first."
        );

        return;
    }

    bool printed =
        BillingManager::PrintInvoice(
            invoiceID
        );

    if (!printed)
    {
        qDebug()
            << "Invoice printing was cancelled "
            "or failed.";
    }
}

