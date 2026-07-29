#pragma once
#include <QSqlQuery>
#include <QSqlError>
#include <QList>

class User;

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
    Permission(Type type); // automatically convert enum Type to Permission
    operator Type() const; // automatically convert Permission to Type
    static bool initTable();
    static bool changeUserPermission(User& other, Permission per, bool isOn);
    static QList<Permission> GetActiveUserPermission();
    int toUnderlying() const; // convert enum type to int, for example, Permission::viewlog -> int(0)
private:
    Type Val;
};