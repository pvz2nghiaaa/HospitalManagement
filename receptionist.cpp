#include "receptionist.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
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

    int currentUserId =
        User::GetActiveUser().GetID();

    QSqlQuery query;

    query.prepare(
        "SELECT LogID, Date, IsPresent, EmployeeID "
        "FROM AttendanceLogs "
        "WHERE EmployeeID = :empId "
        "AND Date BETWEEN :dateFrom AND :dateTo "
        "ORDER BY Date DESC"
    );

    query.bindValue(":empId", currentUserId);
    query.bindValue(":dateFrom", dateFrom);
    query.bindValue(":dateTo", dateTo);

    if (query.exec())
    {
        while (query.next())
        {
            logs.append(
                AttendanceLog(
                    query.value("LogID").toInt(),
                    query.value("Date").toString(),
                    query.value("IsPresent").toInt(),
                    query.value("EmployeeID").toInt()
                )
            );
        }
    }
    else
    {
        qDebug() << "Failed to search receptionist logs:"
            << query.lastError().text();
    }

    return logs;
}