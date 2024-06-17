#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <algorithm>
#include <exception>

template<class Tuple, size_t count>
struct print_element {
    static std::ostream& print(std::ostream& s, const Tuple& t) {
        return print_element<Tuple, count - 1>::print(s, t) << (count == 1 ? "" : ", ") << std::get<count - 1>(t);
    }
};
template <class Tuple>
struct print_element<Tuple, 0> {
    static std::ostream& print(std::ostream& s, const Tuple&) {
        return s;
    }
};
template <class Tuple>
void print_tuple(std::ostream& s, const Tuple& t) {
    print_element<Tuple, std::tuple_size<Tuple>::value>::print(s, t);
}

template<std::size_t I, class... Args>
void print_tuple1(std::ostream& s, const std::tuple<Args...>& t) {
    if constexpr (I < sizeof...(Args)) {
        s << std::get<I>(t);
        if constexpr (I + 1 != sizeof...(Args)) {
            s << ", ";
        }
        print_tuple1<I + 1>(s, t);
    }
}

template<class Tuple, std::size_t... I>
void print_tuple2(std::ostream& s, const Tuple& t, std::index_sequence<I ...>) {
    ((s << (I == 0 ? "" : ", ") << std::get<I>(t) ), ... );
}

template<class CharT, class Traits, class... Args>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& s, const std::tuple<Args...>& t)
{
    s << "(";
    //print_tuple(s, t); //basic
    //print_tuple1<0>(s, t); //with if constexpr
    print_tuple2(s, t, std::make_index_sequence<sizeof...(Args)>{}); //with fold expr
    return s << ")";
}

//Base version

//template <class... Args>
//class CsvParser0 {
//private:
//	int skip_count;
//	std::istream& stream;
//    
//public:
//	CsvParser0(std::istream& csv_stream, int c) : stream(csv_stream), skip_count(c) { };
//	~CsvParser0() = default;
//
//    class Iterator {
//    private:
//        std::istream& stream;
//        int skip_count;
//        std::tuple<Args...> cur;
//
//        template<std::size_t... I>
//        void read_tuple(std::istream& st, std::index_sequence<I ...>) {
//            ((st >> std::get<I>(cur)), ...);
//        }
//
//    public:
//        Iterator(std::istream& s, int c) : stream(s), skip_count(c) {
//            for (int i = 0; i < skip_count; ++i) {
//                std::string line;
//                std::getline(stream, line);
//            }
//            if (skip_count != -1) {
//                ++(*this);
//            }
//        };
//
//        bool operator!= (const Iterator& it){
//            return !stream.eof() && !it.stream.eof();
//        }
//
//        const Iterator& operator++() {
//            std::string line;
//            if (std::getline(stream, line)) {
//                std::replace(line.begin(), line.end(), ',', ' ');
//                std::stringstream st(line);
//                read_tuple(st, std::make_index_sequence<sizeof...(Args)>{});
//            }
//            return *this;
//        }
//
//        std::tuple<Args...> operator*() const {
//            return cur;
//        }
//
//    };
//
//    Iterator begin() {
//        return Iterator(stream, skip_count);
//    }
//    Iterator end() {
//        return Iterator(stream, -1);
//    }
//};

template<typename T>
struct ParserImpl1 {
    T operator()(const std::string& s) {
        return T();
    }
};
template<>
struct ParserImpl1<int> {
    int operator() (const std::string& s) {
        return stoi(s);
    }
};
template<>
struct ParserImpl1<std::string> {
    std::string operator()(const std::string& s) {
        return s;
    }
};
template<>
struct ParserImpl1<double> {
    double operator() (const std::string& s) {
        return stod(s);
    }
};

template<typename T>
struct ParserImpl2 {
    T operator()(const std::string& s) {
        std::stringstream st(s);
        T element;
        st >> element;
        return element;
    }
};

template<template<typename> typename Parser, typename... Args>
struct Converter {
    template<size_t I, typename TupleC>
    static void parse_args(std::istream& st, TupleC& cur) {
        using T = std::tuple_element_t<I, std::tuple<Args...>>;
        Parser<T> p;
        std::string element;
        getline(st, element, ',');
        T x = p(element);
        std::get<I>(cur) = x;
        if constexpr (I < sizeof...(Args) - 1) {
            parse_args<I + 1>(st, cur);
        }
    }
};

template <template<typename> typename ParserImpl, class... Args>
class CsvParser {
private:
	int skip_count;
	std::istream& stream;
    
public:
	CsvParser(std::istream& csv_stream, int c) : stream(csv_stream), skip_count(c) { };
	~CsvParser() = default;

    class Iterator {
    private:
        std::istream& stream;
        int skip_count;
        std::tuple<Args...> cur;

        template<std::size_t... I>
        void read_tuple(std::istream& st, std::index_sequence<I ...>) {
            ((st >> std::get<I>(cur)), ...);
        }

    public:
        Iterator(std::istream& s, int c) : stream(s), skip_count(c) {
            for (int i = 0; i < skip_count; ++i) {
                std::string line;
                std::getline(stream, line);
            }
            if (skip_count != -1) {
                ++(*this);
            }
        };

        bool operator!= (const Iterator& it){
            return !stream.eof() && !it.stream.eof();
        }

        const Iterator& operator++() {
            std::string line;
            if (std::getline(stream, line)) {
                std::stringstream st(line);
                Converter<ParserImpl, Args...>::parse_args<0>(st, cur);
            }
            return *this;
        }

        std::tuple<Args...> operator*() const {
            return cur;
        }

    };

    Iterator begin() {
        return Iterator(stream, skip_count);
    }
    Iterator end() {
        return Iterator(stream, -1);
    }
};
struct A {
    int x = 0;
    int y = 2;
};

int main() {
	//std::tuple<int, std::string, double> t = { 5, "abcd", 3.14};
	//// выводит (5, abcd, 3.14). Вместо std::cout может быть файловый поток (ofstream)
	//std::cout << t;

    std::ifstream csv_stream("C:/Users/vk892/OOP_2c/Ekaterina/lab4.2(visial)/test.txt");
    if (csv_stream.is_open()) {
        try {
            CsvParser<ParserImpl2, int, std::string, double> parser(csv_stream, 0 /*skip first lines count*/);
            for (std::tuple<int, std::string, double> rs : parser) {
                std::cout << rs << "\n";
            }
        }
        catch (const std::exception& err) {
            std::cerr << err.what() << std::endl;
            return 1;
        }
    }
    else {
        std::cout << "cannot open file" << std::endl;
    }

    //try {
    //    CsvParser<int, std::string, double> parser(std::cin, 0 /*skip first lines count*/);
    //    for (std::tuple<int, std::string, double> rs : parser) {
    //        std::cout << rs << "\n";
    //    }
    //}
    //catch (const std::exception& err) {
    //    std::cerr << err.what() << std::endl;
    //    return 1;
    //}

	return 0;
}