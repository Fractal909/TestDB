#include "employee.h"


Employee::Employee(std::string&& name, std::string&& date_of_birth, char gender)
    :name_(std::move(name)), date_of_birth_(std::move(date_of_birth)), gender_(gender)
{
}

int Employee::GetAge() const {
    return 0;
}

bool Employee::AddToDB(sqlite3* const db) const {

    // Подготовка SQL-запроса
    std::string sql = "INSERT INTO Employees (full_name, birth_date_JDN, gender) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error of request preparation: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Привязка параметров
    sqlite3_bind_text(stmt, 1, name_.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, GregToJDN(date_of_birth_));
    sqlite3_bind_text(stmt, 3, &gender_, 1, SQLITE_STATIC);

    // Выполнение запроса
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Error of data inserting: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    std::cout << "Employee has been added!\n";
    return true;
}

double Employee::GregToJDN(const std::string& date) const {

    int year = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day = std::stoi(date.substr(8, 2));

    //Проверить, нужно ли
    if (month <= 2) {
        year--;
        month += 12;
    }

    double JDN = (1461 * (year + 4800 + (month - 14)/12))/4 + 
                 (367 * (month - 2 - 12 * ((month - 14)/12)))/12 - 
                 (3 * ((year + 4900 + (month - 14)/12)/100))/4 + day - 32075;

    return JDN;
}

std::string Employee::GetName() const {
    return name_;
}

std::string Employee::GetBirthDateString() const {
    return date_of_birth_;
}

double Employee::GetBirthDateDouble() const {
    return GregToJDN(date_of_birth_);
}

char Employee::GetGender() const {
    return gender_;
}