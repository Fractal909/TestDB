cmake_minimum_required(VERSION 3.10)
project(TestDB LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TestDB 
                src/main.cpp 
                src/employee.cpp
                src/generator.cpp
                src/mass_applyer.cpp
                third_party/SQLiteCpp-3.3.3/sqlite3/sqlite3.c)

include_directories( include
                    third_party/SQLiteCpp-3.3.3/include/SQLiteCpp
                    third_party/SQLiteCpp-3.3.3/sqlite3)