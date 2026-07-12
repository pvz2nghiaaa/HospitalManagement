#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    bool checkConnection();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    bool IsDBLoaded = false;
    QSqlDatabase DB_Connection;
};
#endif // MAINWINDOW_H
