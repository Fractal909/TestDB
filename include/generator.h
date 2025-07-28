#pragma once

#include "employee.h"
#include <vector>
#include <random>

class Generator {
public:
    Generator(std::vector<Employee>& employees);
private:
    std::string StringGen(size_t length, std::mt19937& gen);

    std::vector<Employee>& employees_;
    std::random_device rd_;
    std::mt19937 gen_;
};