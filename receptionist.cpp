#include "receptionist.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDate>
#include <algorithm>
Receptionist::Receptionist() {}

bool Receptionist::registerNewPatient(QString patientName)
{
    if (!User::GetActiveUser().hasPermission(Permission::createPatient))
    {
        qDebug() << "Access Denied: Current user missing 'createPatient' permission.";
        return false;
    }

    qDebug() << "New patient registered:" << patientName;
    return true;
}

// MY PROFILE
User& Receptionist::GetMyProfileInfo()
{
    return User::GetActiveUser();
}


QList<Permission> Receptionist::GetMyPermissions()
{
    return Permission::GetActiveUserPermission();
}


QList<AttendanceLog> Receptionist::SearchMyActivityLogs(
    const QString& dateFrom,
    const QString& dateTo)
{
    QList<AttendanceLog> logs;

    int currentUserId = User::GetActiveUser().GetID();
    QSqlQuery query;
    query.prepare("SELECT LogID, Date, IsPresent, EmployeeID "
                  "FROM AttendanceLogs "
                  "WHERE EmployeeID = :empId");
    query.bindValue(":empId", currentUserId);

    QDate fromDate = QDate::fromString(dateFrom, "dd-MM-yyyy");
    QDate toDate = QDate::fromString(dateTo, "dd-MM-yyyy");

    if (query.exec())
    {
        while (query.next())
        {
            QString dbDateStr = query.value("Date").toString();
            QDate logDate = QDate::fromString(dbDateStr, "dd-MM-yyyy");

            if (logDate.isValid() && logDate >= fromDate && logDate <= toDate) {
                logs.append(
                    AttendanceLog(
                        query.value("LogID").toInt(),
                        dbDateStr,
                        query.value("IsPresent").toInt(),
                        query.value("EmployeeID").toInt()
                    )
                );
            }
        }
    }
    else
    {
        qDebug() << "Failed to search receptionist logs:" << query.lastError().text();
    }

    // Sort descending
    std::sort(logs.begin(), logs.end(), [](const AttendanceLog &a, const AttendanceLog &b) {
        QDate da = QDate::fromString(a.getDate(), "dd-MM-yyyy");
        QDate db = QDate::fromString(b.getDate(), "dd-MM-yyyy");
        return da > db;
    });

    return logs;
}