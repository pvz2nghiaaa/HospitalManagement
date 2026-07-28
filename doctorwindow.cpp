#include "doctorwindow.h"
#include "ui_DoctorWindow.h"
#include "user.h"
#include "doctor.h"
#include <QMessageBox>
#include "loginwindow.h"
#include <QFileDialog>

DoctorWindow::DoctorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DoctorWindow)
{
    ui->setupUi(this);
    ui->lblAdmin->setText("Dr. " + User::GetActiveUser().GetFullName() + " (Online)");
    navigateToPage(0, ui->btnDashboard);

    ui->txtRecordID->setReadOnly(true);
    ui->txtRecordID_2->setReadOnly(true);
    ui->txtRecordID_4->setReadOnly(true);
    ui->txtRecordID_3->setReadOnly(true);
    ui->txtRecordID_5->setReadOnly(true);

    ui->tblRecordPrescription->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblMedicalRecords->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
        btn->setProperty("active", false); // "active" properties along with its style is declared in .ui
        btn->style()->unpolish(btn); // refresh stylesheet
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

        QString statusText = rec.GetIsComplete() ? "Completed" : "Pending";
        ui->tblMedicalRecords->setItem(i, 5, new QTableWidgetItem(statusText));

        ui->tblMedicalRecords->item(i, 0)->setTextAlignment(Qt::AlignCenter);
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