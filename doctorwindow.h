#ifndef DOCTORWINDOW_H
#define DOCTORWINDOW_H

#include <QMainWindow>
#include <QPushButton>

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

private:
    Ui::DoctorWindow *ui;
    void navigateToPage(int pageIndex, QPushButton* activeBtn);
};

#endif // DOCTORWINDOW_H
