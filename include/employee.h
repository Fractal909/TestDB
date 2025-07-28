#pragma once

#include <iostream>
#include <string>
#include "sqlite3.h"

class Employee {
public:
    Employee(std::string&& name, std::string&& date_of_birth, char gender);
    int GetAge() const;
    bool AddToDB(sqlite3* const db) const;

    std::string GetName() const;
    std::string GetBirthDateString() const;
    double GetBirthDateDouble() const;
    char GetGender() const;

private:
    double GregToJDN(const std::string& date) const;

    std::string name_;
    std::string date_of_birth_;
    char gender_;
};