#include "doctor.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

QList<Patient> Doctor::SearchPatientBy(const QString& keyword) {
    QList<Patient> results;
    QSqlQuery query;

    // 1. Lấy ID của Bác sĩ đang đăng nhập hiện tại
    int currentDoctorId = User::GetActiveUser().GetID();

    // 2. Gom nhóm (FullName OR ID) bằng dấu ngoặc, sau đó mới AND với DoctorID
    query.prepare("SELECT DISTINCT p.* FROM Patients p "
                  "JOIN MedicalRecords m ON p.ID = m.PatientID "
                  "JOIN Diagnoses d ON m.RecordID = d.RecordID "
                  "WHERE (p.FullName LIKE :search OR p.ID = :id) "
                  "AND d.DoctorID = :doctorId");
    query.bindValue(":search", "%" + keyword + "%");
    query.bindValue(":id", keyword.toInt());
    query.bindValue(":doctorId", currentDoctorId);

    if (query.exec()) {
        while (query.next()) {
            Patient p;
            p.ID = query.value("ID").toInt();
            p.FullName = query.value("FullName").toString();
            p.BirthDate = query.value("BirthDate").toString();
            // p.Phone = query.value("PhoneNumber").toString();
            p.Sex = query.value("Sex").toString();
            p.Address = query.value("Address").toString();
            results.append(p);
        }
    } else {
        qDebug() << "Search Error SQL:" << query.lastError().text();
    }

    return results;
}

Patient Doctor::GetPatientDetails(int patientID) {
    Patient p;
    QSqlQuery query;
    query.prepare("SELECT * FROM Patients WHERE ID = :id");
    query.bindValue(":id", patientID);

    if (query.exec() && query.next()) {
        p.ID = query.value("ID").toInt();
        p.FullName = query.value("FullName").toString();
        p.BirthDate = query.value("BirthDate").toString();
        // p.Phone = query.value("Phone").toString();
        p.Sex = query.value("Sex").toString();
        p.Address = query.value("Address").toString();
    }
    return p;
}

bool Doctor::UpdatePatientInfo(int id, const QString& fullName, const QString& birthDate, const QString& sex, const QString& address) {

    if (!User::GetActiveUser().hasPermission(Permission::editPatient)) {
        qDebug() << "No permission to editPatient!";
        return false;
    }
    QSqlQuery query;
    query.prepare("UPDATE Patients SET FullName = :name, BirthDate = :dob, Sex = :sex, Address = :addr WHERE ID = :id");
    query.bindValue(":name", fullName);
    query.bindValue(":dob", birthDate);
    query.bindValue(":sex", sex);
    query.bindValue(":addr", address);
    query.bindValue(":id", id);

    if (query.exec()) {
        return true;
    } else {
        qDebug() << "Update Error SQL:" << query.lastError().text();
        return false;
    }
}
