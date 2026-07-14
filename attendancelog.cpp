#include "attendancelog.h"
#include <QDebug>


AttendanceLog::AttendanceLog()
    : id(-1), date(""), isPresent(false), employeeId(-1) {}

AttendanceLog::AttendanceLog(int id, const QString &date, int isPresent, const int &employeeId)
    : id(id), date(date), isPresent(isPresent != 0), employeeId(employeeId) {}

int AttendanceLog::getId() const {
    return id;
}

QString AttendanceLog::getDate() const {
    return date;
}

void AttendanceLog::setDate(const QString &newDate) {
    date = newDate;
}

int AttendanceLog::getIsPresent() const {
    return isPresent ? 1 : 0;
}

void AttendanceLog::setIsPresent(int present) {
    isPresent = (present != 0);
}

int AttendanceLog::getEmployeeId() const {
    return employeeId;
}

void AttendanceLog::setEmployeeId(const int &empId) {
    employeeId = empId;
}

bool AttendanceLog::initTable() {
    QSqlQuery query;
    bool success = query.exec("CREATE TABLE IF NOT EXISTS AttendanceLogs ("
                              "LogID INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "Date TEXT NOT NULL, "
                              "IsPresent INTEGER NOT NULL, "
                              "EmployeeID TEXT NOT NULL)");
    if (!success) {
        qDebug() << "Fail to create AttendanceLogs table:" << query.lastError().text();
    }
    return success;
}

bool AttendanceLog::save() {
    QSqlQuery query;

    if (this->id == -1) {
        query.prepare("INSERT INTO AttendanceLogs (Date, IsPresent, EmployeeID) "
                      "VALUES (:date, :isPresent, :employeeId)");
        query.bindValue(":date", this->date);
        query.bindValue(":isPresent", this->getIsPresent());
        query.bindValue(":employeeId", this->employeeId);

        if (query.exec()) {
            this->id = query.lastInsertId().toInt();
            return true;
        }
    } else {
        query.prepare("UPDATE AttendanceLogs SET Date = :date, IsPresent = :isPresent, "
                      "EmployeeID = :employeeId WHERE LogID = :id");
        query.bindValue(":date", this->date);
        query.bindValue(":isPresent", this->getIsPresent());
        query.bindValue(":employeeId", this->employeeId);
        query.bindValue(":id", this->id);

        if (query.exec()) {
            return true;
        }
    }

    qDebug() << "Failed to AttendanceLog:" << query.lastError().text();
    return false;
}

AttendanceLog* AttendanceLog::getById(int searchId) {
    QSqlQuery query;
    query.prepare("SELECT LogID, Date, IsPresent, EmployeeID FROM AttendanceLogs WHERE LogID = :id");
    query.bindValue(":id", searchId);

    if (query.exec() && query.next()) {
        return new AttendanceLog(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toInt(),
            query.value(3).toInt()
            );
    }
    return nullptr;
}

QList<AttendanceLog> AttendanceLog::getByEmployeeId(const int &empId) {
    QList<AttendanceLog> list;
    QSqlQuery query;
    query.prepare("SELECT LogID, Date, IsPresent, EmployeeID FROM AttendanceLogs WHERE EmployeeID = :empId");
    query.bindValue(":empId", empId);

    if (query.exec()) {
        while (query.next()) {
            list.append(AttendanceLog(
                query.value(0).toInt(),
                query.value(1).toString(),
                query.value(2).toInt(),
                query.value(3).toInt()
                ));
        }
    } else {
        qDebug() << "Failed to retrieve logs by EmployeeID:" << query.lastError().text();
    }
    return list;
}