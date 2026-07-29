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

    void on_btnSearch_4_clicked();
    void on_tblPatient_cellClicked(int row, int column);
    void on_btnUpdatePatient_clicked();
    void on_btnNewDisease_clicked();

    void on_btnSaveDiagnosis_clicked();

    void on_btnComplete_clicked();

private:
    Ui::DoctorWindow *ui;
    void navigateToPage(int pageIndex, QPushButton* activeBtn);
    int currentRecordId = -1;
    QList<Patient> currentPatientList;
};

#endif