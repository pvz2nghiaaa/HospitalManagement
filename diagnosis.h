class Diagnosis {
public:
    static bool initTable() {
        QSqlQuery q;
        return q.exec("CREATE TABLE IF NOT EXISTS Diagnoses ("
                      "DiagnosisID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "ConditionName TEXT, ICDCode TEXT, Severity TEXT, DateDiagnosed TEXT, "
                      "DoctorID INTEGER, RecordID INTEGER, "
                      "LabTestID INTEGER, LabTestResult TEXT, "
                      "FOREIGN KEY(DoctorID) REFERENCES Users(ID), "
                      "FOREIGN KEY(RecordID) REFERENCES MedicalRecords(RecordID), "
                      "FOREIGN KEY(LabTestID) REFERENCES LabTests_Catalog(LabTestID))");
    }
};