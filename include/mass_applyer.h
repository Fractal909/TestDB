#pragma once

#include "sqlite3.h"
#include "employee.h"

#include <vector>

class MassApplyer {
public:
    MassApplyer(const std::vector<Employee>& employees, sqlite3* const db);
private:
    const std::vector<Employee>& employees_;
    sqlite3* const db_;
};