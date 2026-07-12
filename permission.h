#ifndef PERMISSION_H
#define PERMISSION_H

enum class Permission : int {
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

#endif // PERMISSION_H
