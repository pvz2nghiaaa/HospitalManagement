#ifndef DOCTORWINDOW_H
#define DOCTORWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QList>
#include "doctor.h"
#include "patient.h"


namespace Ui {
class DoctorWindow;
}

class DoctorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DoctorWindow(QWidget *parent = nullptr);
    ~DoctorWindow();

private slots:
    void on_btnDashboard_clicked();
    void on_btnPatients_clicked();
    void on_btnDoctors_clicked();
    void on_btnLogout_clicked();

<<<<<<< HEAD
    void on_btnSearch_5_clicked();
    void on_btnRefreshRecords_clicked();
    void on_tblMedicalRecords_cellClicked(int row, int column);
    void on_btnPrintRecord_clicked();
    void on_btnCloseRecord_clicked();
=======
    void on_btnSearch_4_clicked();
    void on_tblPatient_cellClicked(int row, int column);
    void on_btnUpdatePatient_clicked();
>>>>>>> f196fc5d079b65f1fcd71665049ee813be7d7697

private:
    Ui::DoctorWindow *ui;
    void navigateToPage(int pageIndex, QPushButton* activeBtn);

    QList<Patient> currentPatientList;
};

#endif
