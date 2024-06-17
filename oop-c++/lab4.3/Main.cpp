#include "Header.h"
#include <list>

template <class Type = void>
struct less
{
    bool operator()(const Type& Left, const Type& Right) const;
};

template <>
struct less<A> 
{
    bool operator()(const A& Left, const A& Right) const {
        return ((Left.x < Right.x) && (Left.y < Right.y));
    };
};

template <class Type = void>
struct equal
{
    bool operator()(const Type& Left, const Type& Right) const;
};

template <>
struct equal<A> 
{
    bool operator()(const A& Left, const A& Right) const {
        return ((Left.x == Right.x) && (Left.y == Right.y));
    };
};

int main(void) {
    flat_map::FlatMap < int, int > studenti;
    studenti[1] = 0;
    //std::cout << studenti[1] << std::endl;
    flat_map::FlatMap < std::string, int > studentsi;
    studentsi["Ivan"] = 1;
    studentsi["dvsv"] = 3;
    std::cout << studentsi["Ivan"] << std::endl;
    std::cout << studentsi.contains("Ivan") << std::endl;
    std::cout << studentsi.size() << std::endl;
    std::cout << studentsi["dvsv"] << std::endl;

    for (auto it = studentsi.begin(); it != studentsi.end(); it++) {
        std::cout << it->first << " " << it->second << std::endl;
    }

    flat_map::FlatMap < std::string, std::string, std::less<std::string>, std::equal_to<std::string>, CustomAllocator<std::pair<std::string, std::string>>> student;
    student["first_name"] = "Ivan";
    std::cout << student["first_name"] << std::endl;
    student["last_name"] = "Petrov";
    student["university"] = "NSU";
    student["department"] = "FIT";
    student["count_of_people"] = "12";
    student["group"] = "22205";
    student["oop"] = "2";
    student["zfgd"] = "dvdbeb ";
    flat_map::FlatMap < std::string, std::string, std::less<std::string>, std::equal_to<std::string>, CustomAllocator<std::pair<std::string, std::string>>> student2(student);
    std::cout << student2["oop"] << std::endl;
    student["kfvmkfed"] = "fjv";
    std::cout << student["group"] << std::endl;
    student2.clear();
    std::cout << student2.contains("group") << " " << student2.size() << std::endl;

    A a1 {1, 3};
    A a2 {1, 2};
    A a3 {3, 1};
    A a4 {2, 1};
    flat_map::FlatMap < A, int, less< A >, equal<A>, CustomAllocator<std::pair<A, int>>> studenta;
    studenta[a1] = 1;
    studenta[a2] = 1;
    studenta[a3] = 0;
    std::cout << studenta.contains(a2) << " " << studenta.size() << std::endl;
    std::cout << studenta.contains(a4) << " " << studenta.size() << std::endl;


    union ticks { 
        unsigned long long t64;
        struct s32 { long th, tl; } t32;
    } start, end;

    const size_t list_capacity = 10;
    std::list<int, CustomAllocator<int>> custom_list({ 1, 2, 7, 9, 15 }, CustomAllocator<int>(list_capacity));
    std::list<int> s_list = { 1, 2, 7, 9, 15 };

    double min_time = 10000000;
    //begin
    for (int j = 0; j < 10; j++) {
        asm("rdtsc\n":"=a"(start.t32.th),"=d"(start.t32.tl));
        for (int i = 0; i < 1000; i++) {
            s_list.push_front(i);
        }
        asm("rdtsc\n":"=a"(end.t32.th),"=d"(end.t32.tl));
        min_time = std::min((double) end.t64-start.t64, min_time);
    }
    std::cout << "Time taken s_list push_front: " << min_time/1000 << "sec." << std::endl;
    min_time = 10000000;
    for (int j = 0; j < 10; j++) {
        asm("rdtsc\n":"=a"(start.t32.th),"=d"(start.t32.tl));
        for (int i = 0; i < 1000; i++) {
            custom_list.push_front(i);
        }
        asm("rdtsc\n":"=a"(end.t32.th),"=d"(end.t32.tl));
        min_time = std::min((double) end.t64-start.t64, min_time);
    }
    std::cout << "Time taken custom_list push_front: " << min_time/1000 << "sec." << std::endl;

    //end
    min_time = 10000000;
    for (int j = 0; j < 10; j++) {
        asm("rdtsc\n":"=a"(start.t32.th),"=d"(start.t32.tl));
        for (int i = 0; i < 1000; i++) {
            s_list.push_back(i);
        }
        asm("rdtsc\n":"=a"(end.t32.th),"=d"(end.t32.tl));
        min_time = std::min((double) end.t64-start.t64, min_time);
    }
    std::cout << "Time taken s_list push_back: " << min_time/1000 << "sec." << std::endl;

    min_time = 10000000;
    for (int j = 0; j < 10; j++) {
        asm("rdtsc\n":"=a"(start.t32.th),"=d"(start.t32.tl));
        for (int i = 0; i < 1000; i++) {
            custom_list.push_back(i);
        }
        asm("rdtsc\n":"=a"(end.t32.th),"=d"(end.t32.tl));
        min_time = std::min((double) end.t64-start.t64, min_time);
    }
    std::cout << "Time taken custom_list push_back: " << min_time/1000 << "sec." << std::endl;

    //insert
    min_time = 10000000;
    for (int j = 0; j < 10; j++) {
        auto it2 = s_list.begin();
        asm("rdtsc\n":"=a"(start.t32.th),"=d"(start.t32.tl));
        for (int i = 0; i < 500; i++, it2++) {
            s_list.insert(it2, i);
        }
        for (int i = 0; i < 500; i++) {
            s_list.insert(it2, i);
        }
        asm("rdtsc\n":"=a"(end.t32.th),"=d"(end.t32.tl));
        min_time = std::min((double) end.t64-start.t64, min_time);
    }
    std::cout << "Time taken s_list insert: " << min_time/1000 << "sec." << std::endl;

    min_time = 10000000;
    for (int j = 0; j < 10; j++) {
        auto it1 = custom_list.begin();
        asm("rdtsc\n":"=a"(start.t32.th),"=d"(start.t32.tl));
        for (int i = 0; i < 500; i++, it1++) {
            custom_list.insert(it1, i);
        }
        for (int i = 0; i < 500; i++) {
            custom_list.insert(it1, i);
        }
        asm("rdtsc\n":"=a"(end.t32.th),"=d"(end.t32.tl));
        min_time = std::min((double) end.t64-start.t64, min_time);
    }
    std::cout << "Time taken custom_list insert: " << min_time/1000 << "sec." << std::endl;

    //copy
    std::list<int, CustomAllocator<int>> tmp_custom_list;
    std::list<int> tmp_s_list;
    
    asm("rdtsc\n":"=a"(start.t32.th),"=d"(start.t32.tl));
    std::copy(s_list.begin(), s_list.end(), tmp_s_list.begin());
    asm("rdtsc\n":"=a"(end.t32.th),"=d"(end.t32.tl));
    std::cout << "Time taken s_list copy: " << (double) end.t64-start.t64 << "sec." << std::endl;

    asm("rdtsc\n":"=a"(start.t32.th),"=d"(start.t32.tl));
    std::copy(custom_list.begin(), custom_list.end(), tmp_custom_list.begin());
    asm("rdtsc\n":"=a"(end.t32.th),"=d"(end.t32.tl));
    std::cout << "Time taken custom_list copy: " << (double) end.t64-start.t64 << "sec." << std::endl;

    // for (const auto& element : custom_list) {
    //     std::cout << element << " " ;
    // }
    // for (const auto& element : s_list) {
    //     std::cout << element << " ";
    // }
    // std::cout << std::endl;

    return 0;
}