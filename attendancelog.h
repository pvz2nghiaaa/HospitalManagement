#pragma once
#include <QString>
#include <QList>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDate>

class AttendanceLog {
private:
    int id;
    QString date;
    bool isPresent;
    int employeeId;

public:
    // --- Constructors ---
    AttendanceLog();
    AttendanceLog(int id, const QString &date, int isPresent, const int &employeeId);

    // --- Getters & Setters ---
    int getId() const;
    QString getDate() const;
    void setDate(const QString &newDate);

    int getIsPresent() const;
    void setIsPresent(int present);

    int getEmployeeId() const;
    void setEmployeeId(const int &empId);

    static bool initTable();
    bool save();

    static AttendanceLog* getById(int searchId);
    static QList<AttendanceLog> getByEmployeeId(const int &empId);
    static QList<AttendanceLog> GetRecentLogs();
    static QList<AttendanceLog> GetLogsBetweenDates(const QDate &from, const QDate &to);
    static QList<AttendanceLog> SearchByDate(QString date);
    static QList<AttendanceLog> SearchByRole(QString role);
    static QList<AttendanceLog> SearchByActivity(bool isActive);

    // operator ==
    bool operator ==(const AttendanceLog& other) const {return id == other.id;}
};

