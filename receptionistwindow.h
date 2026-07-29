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

private:
    Ui::ReceptionistWindow *ui;
    void navigateToPage(int pageIndex, QPushButton* activeBtn);
};

#endif // RECEPTIONISTWINDOW_H
