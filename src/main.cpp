#include <ctime>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include "sqlite3.h"
#include "employee.h"
#include "generator.h"
#include "mass_applyer.h"



void CreateTable() {

    sqlite3* db = nullptr;
    int rc = sqlite3_open("employee.db", &db);
    if (rc) {
        std::cerr << "Error to open DB: " << sqlite3_errmsg(db) << std::endl;
    }


    // SQL-запрос для создания таблицы
    const std::string  sql = 
        "CREATE TABLE IF NOT EXISTS Employees("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "full_name TEXT NOT NULL,"
        "birth_date_JDN REAL NOT NULL,"
        "gender CHAR(1) NOT NULL);";


    char* errMsg = nullptr;
    // Выполняем SQL-запрос
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Error to create table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Table 'Employees' has been created!" << std::endl;
    }

    // Закрываем соединение с базой данных
    sqlite3_close(db);
}

void AddEmployee(int argc, char** argv) {

    if (argc != 5) {
        std::cout << argc << std::endl;
        std::cerr << "Incorrect number of arguments";
        return;
    }

    std::string full_name = argv[2];
    std::string birth_date = argv[3];
    char gender = argv[4][0];

    sqlite3* db = nullptr;
    int rc = sqlite3_open("employee.db", &db);
    if (rc) {
        std::cerr << "Error to open DB: " << sqlite3_errmsg(db) << std::endl;
    }


    const Employee emp(std::move(full_name), std::move(birth_date), gender);
    emp.AddToDB(db);

    
    // Закрываем соединение с базой данных
    sqlite3_close(db);
}

void PrintUniqueEmployees() {

    sqlite3* db = nullptr;
    int rc = sqlite3_open("employee.db", &db);
    if (rc) {
        std::cerr << "Error to open DB: " << sqlite3_errmsg(db) << std::endl;
    }


    // SQL-запрос для выборки уникальных записей
    const char* sql = R"(
        SELECT full_name, 
               date(birth_date_JDN) AS birth_date_JDN, 
               gender
        FROM Employees
        GROUP BY full_name, birth_date_JDN
        ORDER BY full_name;
    )";
    
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error of request preparation: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    //Получение текущего года
    std::time_t now = std::time(0);
    std::tm* ltm = std::localtime(&now);
    int current_year = 1900 + ltm->tm_year;
    
    // Вывод заголовка таблицы
    std::cout << "\nUnique employees (by name and date):\n";
    std::cout << "------------------------------------------------------------\n";
    std::cout << std::left << std::setw(30) << "Name" 
              << std::setw(16) << "Date of birth" << std::setw(9) 
              << "Gender" << std::setw(6) << "Age" << std::setw(0);
    std::cout << "\n------------------------------------------------------------\n";
    
    // Перебор результатов
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Извлечение данных из строки результата
        const char* full_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* birth_date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* gender = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        // Форматированный вывод
        std::cout << std::left << std::setw(30) << full_name
                  << std::setw(16) << birth_date << std::setw(9) 
                  << gender << std::setw(6)  
                  << current_year - std::stoi(std::string(birth_date).substr(0, 4)) 
                  << std::setw(0) << std::endl;
    }
    std::cout << "\n------------------------------------------------------------\n";
    
    sqlite3_finalize(stmt);
}

void MassGeneration() {

    sqlite3* db = nullptr;
    int rc = sqlite3_open("employee.db", &db);
    if (rc) {
        std::cerr << "Error to open DB: " << sqlite3_errmsg(db) << std::endl;
    }


    std::vector<Employee> employees;
    employees.reserve(1000);
    Generator gen(employees);
    MassApplyer(employees, db);
}

void CreateOptimalIndex(sqlite3* db) {
    const char* sql = "CREATE INDEX IF NOT EXISTS idx_gender_surname ON Employees(gender, full_name);";
    sqlite3_exec(db, sql, 0, 0, 0);
}

void GetMEmployees() {

    sqlite3* db = nullptr;
    int rc = sqlite3_open("employee.db", &db);
    if (rc) {
        std::cerr << "Error to open DB: " << sqlite3_errmsg(db) << std::endl;
    }


    // Создаем оптимальный индекс
    auto start_idx = std::chrono::high_resolution_clock::now();
    CreateOptimalIndex(db);
    auto end_idx = std::chrono::high_resolution_clock::now();

    // Выполняем запрос
    const char* sql = "SELECT id, full_name, date(birth_date_JDN) "
                      "FROM Employees "
                      "WHERE gender = 'M' AND full_name LIKE 'F%' "
                      "ORDER BY full_name;";

    auto start_query = std::chrono::high_resolution_clock::now();
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        count++; // Просто подсчет, без обработки данных
    }
    
    auto end_query = std::chrono::high_resolution_clock::now();
    
    // Результаты
    auto idx_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_idx - start_idx);
    auto query_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_query - start_query);
    
    std::cout << "Index creation time: " << idx_time.count() << " ms\n";
    std::cout << "Query execution time: " << query_time.count() << " ms\n";
    std::cout << "Employee found: " << count << "\n";
}



int main(int argc, char** argv) {

    if (argc < 2) {
        std::cerr << "At least one argument required!";
        return 0;
    }

    if (!isdigit(*argv[1])) {
        std::cerr << "Invalid argument!";
        return 0;
    }

    int mode = std::stoi(argv[1]);

    std::cout << "Mode - " << mode << std::endl;

    switch (mode)
    {
    case 1:
        CreateTable();
        break;
    case 2:
        AddEmployee(argc, argv);
        break;
    case 3:
        PrintUniqueEmployees();
        break;
    case 4:
        MassGeneration();
        break;
    case 5:
        GetMEmployees();
        break;
    default:
        break;
    }

    return 0;
}