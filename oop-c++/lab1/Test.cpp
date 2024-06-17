#include "Header.h"
#include "gtest/gtest.h"

namespace {

    TEST(Test, access_insertions_size_contains) {
        using flat_map::FlatMap;
        FlatMap student;
        student["first_name"] = "Ivan";
        student["last_name"] = "Petrov";
        student["university"] = "NSU";
        student["department"] = "FIT";
        student["count_of_people"] = "12";
        student["group"] = "22205";

        EXPECT_EQ(student["first_name"], "Ivan");
        EXPECT_EQ(student["last_name"], "Petrov");
        EXPECT_EQ(student.size(), 6);
        EXPECT_EQ(student.contains("university"), true);
        EXPECT_EQ(student.contains("people"), false);
    }

    TEST(Test, erase_size_contains) {
        using flat_map::FlatMap;
        FlatMap student;
        student["first_name"] = "Ivan";
        student["last_name"] = "Petrov";
        student["university"] = "NSU";
        student["department"] = "FIT";
        student["count_of_people"] = "12";
        student["group"] = "22205";

        EXPECT_EQ(student.contains("count_of_people"), true);

        student.erase("count_of_people");
        EXPECT_EQ(student.size(), 5);
        student.erase("last_name");
        EXPECT_EQ(student.size(), 4);
        EXPECT_EQ(student.contains("count_of_people"), false);
    }

    TEST(Test, copy) {
        using flat_map::FlatMap;
        FlatMap student;
        student["first_name"] = "Ivan";
        student["last_name"] = "Petrov";
        student["university"] = "NSU";
        student["department"] = "FIT";
        student["count_of_people"] = "12";
        student["group"] = "22205";

        FlatMap person(student);
        EXPECT_EQ(person.size(), 6);

        person["status"] = "student";
        EXPECT_EQ(person.size(), 7);
    }

    TEST(Test, assignment) {
        using flat_map::FlatMap;
        FlatMap student;
        student["first_name"] = "Ivan";
        student["last_name"] = "Petrov";
        student["university"] = "NSU";
        student["department"] = "FIT";
        student["count_of_people"] = "12";
        student["group"] = "22205";

        FlatMap person;
        person = student;
        EXPECT_EQ(person.size(), 6);

        person["status"] = "student";
        EXPECT_EQ(person.size(), 7);
    }

    TEST(Test, clear) {
        using flat_map::FlatMap;
        FlatMap student;
        student["first_name"] = "Ivan";
        student["last_name"] = "Petrov";
        student["university"] = "NSU";
        student["department"] = "FIT";
        student["count_of_people"] = "12";
        student["group"] = "22205";

        student.clear();
        EXPECT_EQ(student.size(), 0);
        EXPECT_EQ(student.contains("group"), false);
    }

    TEST(Test, move_constructor) {
        using flat_map::FlatMap;
        FlatMap student;
        student["first_name"] = "Ivan";
        student["last_name"] = "Petrov";
        student["university"] = "NSU";
        student["department"] = "FIT";
        student["count_of_people"] = "12";
        student["group"] = "22205";

        FlatMap person(std::move(student));
        EXPECT_EQ(person.size(), 6);

        person["status"] = "student";
        EXPECT_EQ(person.size(), 7);
    }

    TEST(Test, assignment_operator_with_displacement) {
        using flat_map::FlatMap;
        FlatMap student;
        student["first_name"] = "Ivan";
        student["last_name"] = "Petrov";
        student["university"] = "NSU";
        student["department"] = "FIT";
        student["count_of_people"] = "12";
        student["group"] = "22205";

        FlatMap person;
        person = std::move(student);
        EXPECT_EQ(person.size(), 6);

        person["status"] = "student";
        EXPECT_EQ(person.size(), 7);
    }

    TEST(Test, iterator) {
        using flat_map::FlatMap;
        FlatMap student;
        student["first_name"] = "Ivan";
        student["last_name"] = "Petrov";
        student["university"] = "NSU";
        student["department"] = "FIT";
        student["count_of_people"] = "12";
        student["group"] = "22205";

        for (auto it = student.begin(); it != student.end(); it++) {
            EXPECT_EQ(it->value, student[it->key]);
        }

        auto it = student.begin();
        it += 1;
        EXPECT_EQ(it->value, "FIT");

    }

    TEST(Test, find) {
        using flat_map::FlatMap;
        FlatMap student;
        student["first_name"] = "Ivan";
        student["last_name"] = "Petrov";
        student["university"] = "NSU";
        student["department"] = "FIT";
        student["count_of_people"] = "12";
        student["group"] = "22205";

        auto it = student.find("group");
        EXPECT_EQ(it->key, "group");
        EXPECT_EQ(it->value, "22205");

        it = student.find("g");
        EXPECT_EQ(it->key, "");
        EXPECT_EQ(it->value, "");
    }

}