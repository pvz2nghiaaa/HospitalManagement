class PrescriptionDetail {
public:
    static bool initTable() {
        QSqlQuery q;
        return q.exec("CREATE TABLE IF NOT EXISTS PrescriptionDetails ("
                      "DetailID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "Quantity INTEGER, Note TEXT, DrugID INTEGER, RecordID INTEGER, "
                      "FOREIGN KEY(DrugID) REFERENCES Drugs(DrugID), "
                      "FOREIGN KEY(RecordID) REFERENCES MedicalRecords(RecordID), "
                      "CONSTRAINT uq_prescription UNIQUE(RecordID, DrugID))");
    }
};