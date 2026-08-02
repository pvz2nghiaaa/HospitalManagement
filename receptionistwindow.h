#ifndef RECEPTIONISTWINDOW_H
#define RECEPTIONISTWINDOW_H

#include <QMainWindow>
#include <QPushButton>

namespace Ui {
    class ReceptionistWindow;
}

class ReceptionistWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReceptionistWindow(QWidget* parent = nullptr);
    ~ReceptionistWindow();

private slots:
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
    // Drug Management
    void on_btnSearch_14_clicked();
    void on_btnSearch_15_clicked();
    void on_btnSearch_16_clicked();
    void on_btnSearch_17_clicked();
    void on_btnSearch_18_clicked();
    void on_tblPatient_2_cellClicked(int row, int column);

    void on_btnNewPatient_clicked();

    void on_btnCancelPat_clicked();

    void on_btnSavePat_clicked();

private:
    Ui::ReceptionistWindow* ui;
    void navigateToPage(int pageIndex, QPushButton* activeBtn);
    void loadAllDrugs();
    void GetDrugStockHistory(int drugID);
    void refreshAttendanceTable();



    void setBackgroundActiveState(const bool activeState);

    void showOverlayPatientFrame();
    void hideOverlayPatientFrame();
};

#endif // RECEPTIONISTWINDOW_H
