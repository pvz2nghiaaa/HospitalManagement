#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();

private slots:
    void on_btnDashboard_clicked();

    void on_btnStaffManagement_clicked();

    void on_btnPermissionManagement_clicked();

    void on_btnActivityLogs_clicked();

    void on_btnReport_clicked();

    void navigateToPage(int pageIndex, QPushButton* activeBtn);

private:
    Ui::AdminWindow *ui;
};

#endif // ADMINWINDOW_H