#include "generator.h"


#include <iomanip>
#include <array>
#include <numeric>

Generator::Generator(std::vector<Employee>& employees) 
    :employees_(employees), gen_(rd_())
{

    const int total_records = 1000000;
    const int special_records = 100;
    const int normal_records = total_records - special_records;

    // Инициализация генератора случайных чисел
    std::uniform_int_distribution<> year_dist(1900, 2025);
    std::uniform_int_distribution<> month_dist(1, 12);
    std::uniform_int_distribution<> day_dist(1, 28);
    std::uniform_int_distribution<> len_dist(5, 10);

    // Буквы для равномерного распределения
    std::array<char, 26> letters;
    std::iota(letters.begin(), letters.end(), 'A');
    std::shuffle(letters.begin(), letters.end(), gen_);

    // Посев для генерации пола
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

     // Генерация обычных записей
    for (int i = 0; i < normal_records; i++) {

        // Генерация ФИО
        char first_letter = letters[i % 26];
        int name_len = len_dist(gen_);
        std::string full_name = first_letter + 
                                StringGen(name_len, gen_) + " " +
                                StringGen(6, gen_) + " " + 
                                StringGen(8, gen_);
        
        // Генерация даты рождения        
        int year = year_dist(gen_);
        int month = month_dist(gen_);
        int day = day_dist(gen_);

        std::string date = std::to_string(year) + '-';
        date += month < 10 ? '0' + std::to_string(month) : std::to_string(month);
        date += day < 10 ? '0' + std::to_string(day) : std::to_string(day);

        // Генерация пола
        char gender = (std::rand() % 2 == 0) ? 'M' : 'F';


        employees_.push_back(Employee(std::move(full_name), std::move(date), gender));
        

        // Прогресс
        if (i % 10000 == 0) {
            std::cout << "Generated: " << i << "/" << normal_records << " employees\r";
            std::cout.flush();
        }
    }

    // Генерация специальных записей (M + Фамилия на "F")
    for (int i = 0; i < special_records; i++) {
        // Генерация ФИО с фамилией на "F"
        int name_len = len_dist(gen_);
        std::string full_name = "F" + StringGen(name_len, gen_) + " " + 
                                      StringGen(6, gen_) + " " + StringGen(8, gen_);
        
        // Генерация даты рождения        
        int year = year_dist(gen_);
        int month = month_dist(gen_);
        int day = day_dist(gen_);

        std::string date = std::to_string(year) + '-';
        date += month < 10 ? '0' + std::to_string(month) : std::to_string(month);
        date += day < 10 ? '0' + std::to_string(day) : std::to_string(day);
        
        // Определение пола (чередование)
        char gender = (std::rand() % 2 == 0) ? 'M' : 'F';

        employees_.push_back(Employee(std::move(full_name), std::move(date), gender));
    }
}

std::string Generator::StringGen(size_t length, std::mt19937& gen) {
    static const char charset[] = 
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
    
    std::string str(length, '\0');
    for (size_t i = 0; i < length; i++) {
        str[i] = charset[dist(gen)];
    }
    return str;
}