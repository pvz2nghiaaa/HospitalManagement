#ifndef RECEPTIONISTWINDOW_H
#define RECEPTIONISTWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "doctor.h"
#include "attendancelog.h"
namespace Ui {
    class ReceptionistWindow;
}

class ReceptionistWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReceptionistWindow(QWidget* parent = nullptr);
    ~ReceptionistWindow();

signals:
    void queryFailed(const QString& errorMessage);

private slots:
    void handleQueryFailed(const QString& errorMessage);

    void on_btnPatient_clicked();

    void on_btnRecord_clicked();

    void on_btnInvoice_clicked();

    void on_btnDrug_clicked();

    void on_btnProfile_clicked();

    void on_btnAttendance_clicked();
    void on_btnSearchAttendance_clicked();
    void on_btnMarkPresent_clicked();
    void on_btnMarkAbsent_clicked();

    void on_btnLogout_clicked();

    void on_btnPatientSearch_clicked();
    //Invoice
    void on_tableWidget_cellClicked(
        int row,
        int column
    );

    void on_btnSearch_10_clicked();
    void on_btnSearch_11_clicked();
    void on_btnSearch_12_clicked();
    void on_btnSearch_13_clicked();

    void on_btnSaveCreateInvoice_clicked();
    void on_btnCancelCreateInvoice_clicked();

    // Drug Management
    void on_btnSearch_14_clicked();
    void on_btnSearch_15_clicked();
    void on_btnSearch_16_clicked();
    void on_btnSearch_17_clicked();
    void on_btnSearch_18_clicked();
    void on_tblPatient_2_cellClicked(int row, int column);

    // Add Drug Card
    void on_btnSaveAddDrug_clicked();
    void on_btnCancelAddDrug_clicked();

    // Edit Drug Card
    void on_btnSaveEditDrug_clicked();
    void on_btnCancelEditDrug_clicked();

    // Drug History Card
    void on_pushButton_clicked();
    void on_btnCloseDrugHistory_clicked();



    void on_btnNewPatient_clicked();

    void on_btnCancelPat_clicked();

    void on_btnSavePat_clicked();
    

    void on_btnSearchActivity_clicked();
    void on_btnRefreshPatient_clicked();

    void on_tblPatient_cellDoubleClicked(int row, int column);

    void on_btnCancelEditPat_clicked();

    void on_btnSaveEditPat_clicked();

    // FrameMedicalRecord features
    void on_txtRecordID_textChanged(const QString &text);
    void on_tblAvaiDoctor_cellClicked(int row, int column);
    void on_btnCreateRec_clicked();
    void on_btnAutoCreateRec_clicked();
    void on_btnClear_clicked();

    void on_btnSearch_5_clicked();

private:
    Ui::ReceptionistWindow* ui;
    void navigateToPage(int pageIndex, QPushButton* activeBtn);
    void loadAllDrugs();
    void GetDrugStockHistory();
    void refreshAttendanceTable();
    
    //Invoice
    void loadInvoicesToTable();
    void loadInvoiceDetails(
        int invoiceID
    );

    void clearInvoiceDetails();

    int getSelectedInvoiceID() const;

    void showOverlayInvoiceFrame();
    void hideOverlayInvoiceFrame();

    // Drug overlay
    void showAddDrugFrame();
    void hideAddDrugFrame();
    void loadAvailableDoctorsToTable(QList<std::pair<int, QString>> doctors = Doctor::getAvailableDoctors());

    void showEditDrugFrame();
    void hideEditDrugFrame();

    void showDrugHistoryFrame();
    void hideDrugHistoryFrame();
    int currentDrugID = -1;

    void setBackgroundActiveState(const bool activeState);

    void showOverlayPatientFrame();
    void hideOverlayPatientFrame();
    //My profile
    void loadMyProfileInfo();
    void loadMyPermissions();
    void loadMyActivityLogs();
    void showOverlayEditPatientFrame();
    void hideOverlayEditPatientFrame();

    void fetchPatient();
};

#endif // RECEPTIONISTWINDOW_H


