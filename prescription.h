#pragma once
#include <QList>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "drug.h"

class Prescription
{
private:
    int diagnosisID;
    QList<Drug> drugs;
    QList<int> quantities;
    QList<QString> notes;

public:
    // Constructors
    Prescription();
    Prescription(int diagnosisID, const QList<Drug> &drugs, const QList<int> &quantities, const QList<QString> &notes);

    // Database initialization
    static bool initTable();

    // Helper method to add a drug item
    void addDrug(const Drug &drug, int quantity, const QString &note = "");

    // Getters
    int getDiagnosisID() const;
    QList<Drug> getDrugs() const;
    QList<int> getQuantities() const;
    QList<QString> getNotes() const;

    // Setters
    void setDiagnosisID(int id);
    void setDrugs(const QList<Drug> &d);
    void setQuantities(const QList<int> &q);
    void setNotes(const QList<QString> &n);

    // Fluent Setters
    Prescription& SetDiagnosisID(int id);
    Prescription& SetDrugs(const QList<Drug> &d);
    Prescription& SetQuantities(const QList<int> &q);
    Prescription& SetNotes(const QList<QString> &n);
};
