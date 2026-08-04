#include "doctor.h"
#include <QSqlQuery>
#include <QVariant>


User& Doctor::GetMyProfileInfo() {

    return User::GetActiveUser();
}

QList<Permission> Doctor::GetMyPermissions() {
    return Permission::GetActiveUserPermission();
}

QList<AttendanceLog> Doctor::SearchMyActivityLogs(const QString& dateFrom, const QString& dateTo) {
    QList<AttendanceLog> logs;

    int currentUserId = User::GetActiveUser().GetID();

    QSqlQuery query;
    query.prepare("SELECT LogID, Date, IsPresent, EmployeeID "
                  "FROM AttendanceLogs "
                  "WHERE EmployeeID = :empId AND Date BETWEEN :dateFrom AND :dateTo");


    query.bindValue(":empId", currentUserId);
    query.bindValue(":dateFrom", dateFrom);
    query.bindValue(":dateTo", dateTo);


    if (query.exec()) {
        while (query.next()) {
            logs.append(AttendanceLog(
                query.value("LogID").toInt(),
                query.value("Date").toString(),
                query.value("IsPresent").toInt(),
                query.value("EmployeeID").toInt()
                ));
        }
    } else {
        qDebug() << "Failed to search activity logs:" << query.lastError().text();
    }

    return logs;
}