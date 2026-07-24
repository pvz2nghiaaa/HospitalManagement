#include "prescription.h"

Prescription::Prescription() : diagnosisID(-1) {}

Prescription::Prescription(int diagnosisID, const QList<Drug> &drugs,
                           const QList<int> &quantities,
                           const QList<QString> &notes)
    : diagnosisID(diagnosisID), drugs(drugs), quantities(quantities),
      notes(notes) {}

bool Prescription::initTable() {
  static bool tableLoaded = false;
  if (!tableLoaded) {
    QSqlQuery query;
    bool ok = query.exec("CREATE TABLE IF NOT EXISTS Prescriptions ("
                         "DetailID INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "Quantity INTEGER, "
                         "Note TEXT, "
                         "DrugID INTEGER, "
                         "DiagnosisID INTEGER, "
                         "CONSTRAINT uq_prescription_diagnosis_drug UNIQUE "
                         "(DiagnosisID, DrugID), "
                         "FOREIGN KEY(DrugID) REFERENCES Drugs(DrugID))");
    if (!ok) {
      qDebug() << "Failed to create PrescriptionDetails table:"
               << query.lastError().text();
    } else {
      query.exec(
          "CREATE UNIQUE INDEX IF NOT EXISTS uq_prescription_diagnosis_drug ON "
          "Prescriptions(DiagnosisID, DrugID)");
      qDebug() << "Prescriptions table is initialized";
    }
    tableLoaded = ok;
    return tableLoaded;
  }
  return true;
}

void Prescription::addDrug(const Drug &drug, int quantity,
                           const QString &note) {
  drugs.append(drug);
  quantities.append(quantity);
  notes.append(note);
}

// Getters
int Prescription::getDiagnosisID() const { return diagnosisID; }
QList<Drug> Prescription::getDrugs() const { return drugs; }
QList<int> Prescription::getQuantities() const { return quantities; }
QList<QString> Prescription::getNotes() const { return notes; }

// Setters
void Prescription::setDiagnosisID(int id) { diagnosisID = id; }
void Prescription::setDrugs(const QList<Drug> &d) { drugs = d; }
void Prescription::setQuantities(const QList<int> &q) { quantities = q; }
void Prescription::setNotes(const QList<QString> &n) { notes = n; }

// Fluent Setters
Prescription &Prescription::SetDiagnosisID(int id) {
  diagnosisID = id;
  return *this;
}
Prescription &Prescription::SetDrugs(const QList<Drug> &d) {
  drugs = d;
  return *this;
}
Prescription &Prescription::SetQuantities(const QList<int> &q) {
  quantities = q;
  return *this;
}
Prescription &Prescription::SetNotes(const QList<QString> &n) {
  notes = n;
  return *this;
}
