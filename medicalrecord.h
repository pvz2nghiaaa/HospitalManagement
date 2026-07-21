class MedicalRecord {
public:
    static bool initTable() {
        QSqlQuery q;
        return q.exec("CREATE TABLE IF NOT EXISTS MedicalRecords ("
                      "RecordID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "Date TEXT, IsComplete BOOLEAN, PatientID INTEGER, "
                      "FOREIGN KEY(PatientID) REFERENCES Patients(ID))");
    }
};