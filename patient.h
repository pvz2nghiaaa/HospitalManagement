class Patient {
public:
    static bool initTable() {
        QSqlQuery q;
        return q.exec("CREATE TABLE IF NOT EXISTS Patients ("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "FullName TEXT, BirthDate TEXT, Sex TEXT, Address TEXT)");
    }
};