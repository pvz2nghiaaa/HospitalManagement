#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "permission.h"
#include "attendancelog.h"
using namespace std;

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

    void on_btnLogout_clicked();

    void updateDashboardInfo();

    // Overlay form helpers and slots
    void on_btnAddStaff_clicked();
    void on_btnAddStaffQuick_clicked();
    void on_btnCancelNewStaff_clicked();
    void on_btnSaveNewStaff_clicked();
    void showOverlayForm();
    void hideOverlayForm();

    // Patient Overlay
    void on_btnRegisterPatientQuick_clicked();
    void on_btnCancelPat_clicked();
    void on_btnSavePat_clicked();
    void showPatientOverlay();
    void hidePatientOverlay();

    // Edit Staff Overlay
    void on_btnEditStaff_clicked();
    void on_btnCancelEditStaff_clicked();
    void on_btnSaveEditStaff_clicked();
    void on_chkChangePassword_toggled(bool checked);
    void showEditStaffOverlay();
    void hideEditStaffOverlay();

    // Delete Staff Overlay
    void on_btnDeleteStaff_clicked();
    void on_btnCancelDeleteStaff_clicked();
    void on_btnConfirmDeleteStaff_clicked();
    void showDeleteStaffOverlay();
    void hideDeleteStaffOverlay();

    // Edit Permission Overlay
    void on_btnEditPermission_clicked();
    void on_btnCancelEditPermission_clicked();
    void on_btnSaveEditPermission_clicked();
    void showEditPermissionOverlay();
    void hideEditPermissionOverlay();

    void on_btnRefreshStaff_clicked();

    void on_btnSearch_clicked();

    void on_btnSearchPermission_clicked();
    void on_btnSearchActivity_clicked();
    void on_btnGenerateReport_clicked();
    void on_btnExportPDF_clicked();
    void on_btnExportReportQuick_clicked();

private:
    Ui::AdminWindow *ui;
    void exportLogsToPDF(const QString &filePath, const QDate &from, const QDate &to);
    void refreshStaffDashboard(vector<tuple<int, QString, QString, QString, bool, QString>> listUser);
    void refreshPermissionTable(vector<tuple<int, QString, QString, QList<Permission> > > list);
    void refreshLogsDashboard(QList<AttendanceLog> list);
    QWidget* createBadgeWidget(const QString& text, const QColor& textColor, const QColor& bgColor);
    QWidget* createPermissionChipsWidget(const QList<Permission>& perms);
};

#endif // ADMINWINDOW_H