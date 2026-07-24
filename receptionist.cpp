#include "receptionist.h"
#include <QDebug>

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
