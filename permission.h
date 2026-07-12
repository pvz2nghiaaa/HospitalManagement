#ifndef PERMISSION_H
#define PERMISSION_H

#include <QSqlQuery>
#include <QSqlError>

class Permission
{
public:
    Permission();
    enum Type : int {
        // system & user management
        viewLog,
        addLog,
        changePermission,
        manageUsers,
        // medical records
        createRecord,
        viewRecord,
        editRecord,
        // patient
        createPatient,
        editPatient,
        // billing and financial
        createInvoice,
        viewInvoice,
        // services (add, edit drugs' info, x-ray fee, etc)
        manageDrugs
    };
    constexpr Permission(Type type);
    constexpr operator Type() const;
    static bool initTable();
private:
    Type Val;
    static bool PermissionLoaded;
};

#endif // PERMISSION_H
