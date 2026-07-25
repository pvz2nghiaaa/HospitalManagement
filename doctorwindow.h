#ifndef DOCTORWINDOW_H
#define DOCTORWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "doctor.h"
#include "attendancelog.h"


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


    void on_btnSearchActivity_clicked();

private:
    Ui::DoctorWindow *ui;
    void navigateToPage(int pageIndex, QPushButton* activeBtn);

    void loadProfileData();
};

#endif // DOCTORWINDOW_H
