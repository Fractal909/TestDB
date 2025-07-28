#include "mass_applyer.h"

MassApplyer::MassApplyer(const std::vector<Employee>& employees, sqlite3* const db) 
    :employees_(employees), db_(db)
{
    
    // Начинаем транзакцию для ускорения вставки
    sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, 0);

    // Подготовка SQL-запроса
    const char* sql = "INSERT INTO Employees (full_name, birth_date_JDN, gender) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error of request preparation: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    for (const auto& employee : employees_) {
        //Привязка параметров
        std::string name = employee.GetName();
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, employee.GetBirthDateDouble());
        char gender = employee.GetGender();
        sqlite3_bind_text(stmt, 3, &gender, 1, SQLITE_STATIC);
        
        // Выполнение запроса
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Inserting error: " << sqlite3_errmsg(db) << std::endl;
        }
        
        // Сброс состояния для следующей вставки
        
        sqlite3_reset(stmt);
    }

    // Финализация и коммит
    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", 0, 0, 0);
    std::cout << "\nSuccessfully generated and applied " << employees_.size() << " employees!" << std::endl;
}