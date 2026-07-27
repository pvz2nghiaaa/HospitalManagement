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
    explicit ReceptionistWindow(QWidget *parent = nullptr);
    ~ReceptionistWindow();

private slots:
    void on_btnDashboard_clicked();

    void on_btnPatients_clicked();

    void on_btnPatients_2_clicked();

    void on_btnPatients_3_clicked();

    void on_btnDoctors_clicked();

    void on_btnLogout_clicked();

    void on_btnPatientSearch_clicked();
 // Drug Management
    void on_btnSearch_14_clicked();
    void on_btnSearch_15_clicked();
    void on_btnSearch_16_clicked();
    void on_btnSearch_17_clicked();
    void on_btnSearch_18_clicked();
    void on_tblPatient_2_cellClicked(int row, int column);

private:
    Ui::ReceptionistWindow *ui;
    void navigateToPage(int pageIndex, QPushButton* activeBtn);

 // Drug Functions
    void SearchDrugsBy(const QString &nameOrID,const QString &stockStatus);

    bool UpdateDrugInfo(int drugID,const QString &name,const QString &unit,double price,int stockQuantity);

    bool AddNewDrug(const QString &name,const QString &unit,double price,int stockQuantity);

    void loadAllDrugs();

    QString getDrugStatus(int stockQuantity) const;

    bool RemoveDrug(int drugID);

    void GetDrugStockHistory(int drugID);
};

#endif // RECEPTIONISTWINDOW_H
