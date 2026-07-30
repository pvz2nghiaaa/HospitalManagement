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

    void on_btnRefreshPatient_clicked();

    void on_tblPatient_cellDoubleClicked(int row, int column);

    void on_btnCancelEditPat_clicked();

    void on_btnSaveEditPat_clicked();

private:
    Ui::ReceptionistWindow* ui;
    void navigateToPage(int pageIndex, QPushButton* activeBtn);
    void loadAllDrugs();
    void GetDrugStockHistory(int drugID);



    void setBackgroundActiveState(const bool activeState);

    void showOverlayPatientFrame();
    void hideOverlayPatientFrame();
    void showOverlayEditPatientFrame();
    void hideOverlayEditPatientFrame();

    void fetchPatient();
};

#endif // RECEPTIONISTWINDOW_H
