#pragma once
#include "user.h"
#include "permission.h"
#include "attendancelog.h"
#include <QString>
#include <QList>

class Receptionist
{
public:
    Receptionist();
    static bool registerNewPatient(QString patientName);

    // My Profile
    static User& GetMyProfileInfo();

    static QList<Permission> GetMyPermissions();

    static QList<AttendanceLog> SearchMyActivityLogs(const QString& dateFrom,const QString& dateTo);
};
