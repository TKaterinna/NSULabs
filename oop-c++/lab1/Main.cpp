#include "Header.h"

int main(void) {
    using flat_map::FlatMap;
    FlatMap student;
    student["first_name"] = "Ivan";
    student["last_name"] = "Petrov";
    student["university"] = "NSU";
    student["department"] = "FIT";
    student["count_of_people"] = "12";
    student["group"] = "22205";


    //flat_map::FlatMap student;

    //student["count_of_people"] = "12";
    //student["group"] = "22205";
    //std::cout << "size " << student.size() << std::endl;

    //flat_map::FlatMap person;
    //person["status"] = "student";
    //std::cout << "size " << person.size() << std::endl;
    //person = student;
    //std::cout << "size " << person.size() << std::endl;

    //std::cout << student.erase("group") << std::endl;
    //std::cout << "size " << student.size() << std::endl;
    //std::cout << "size " << person.size() << std::endl;


    //student["count_of_people"] = "12";

    //student.result();
    //std::cout << "size " << student.size() << std::endl;

    //std::cout << "contains('group') " << student.contains("group") << std::endl;
    //std::cout << "contains('g') " << student.contains("g") << std::endl;
    //std::cout << "contains('count_of_people') " << student.contains("count_of_people") << std::endl;



    //student["group"] = "22205";

    //student.result();
    //std::cout << "size " << student.size() << std::endl;

    //std::cout << "contains('group') " << student.contains("group") << std::endl;
    //std::cout << "contains('g') " << student.contains("g") << std::endl;
    //std::cout << "contains('count_of_people') " << student.contains("count_of_people") << std::endl;

    ////flat_map::FlatMap person;
    //person["status"] = "student";

    //person.result();
    //std::cout << "size " << person.size() << std::endl;

    //person = student;
    //person.result();
    //std::cout << "size " << person.size() << std::endl;

    //person["course"] = "2";
    //person.result();
    //std::cout << "size " << person.size() << std::endl;

    //std::cout << "erase group " << person.erase("group") << std::endl;
    //person.result();
    //std::cout << "size " << person.size() << std::endl;

    //std::cout << "erase count_of_people " << person.erase("count_of_people") << std::endl;
    //person.result();
    //std::cout << "size " << person.size() << std::endl;


    //std::cout << std::endl;
    //person["01"] = "1";
    //person["02"] = "2";
    //person["03"] = "3";
    //person["04"] = "4";
    //person["05"] = "5";
    //person["06"] = "6";
    //person["07"] = "7";
    //person["08"] = "8";
    //person["09"] = "9";
    //person["09"] = "9";
    //std::cout << "size " << person.size() << std::endl;

    //std::cout << "erase 03 " << person.erase("03") << std::endl;
    //person.result();
    //std::cout << "size " << person.size() << std::endl;

    //flat_map::FlatMap p(person);

    //for (auto it = student.begin(); it != student.end(); ++it) {
    //    std::cout << it->key << ": " << it->value << "\n";
    //}

    //auto it = student.find("group");
    //std::cout << it->key << ": " << it->value << "\n";

    //it = student.find("g");
    //std::cout << it->key << ": " << it->value << "\n";

    return 0;
}