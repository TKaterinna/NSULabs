#pragma once
#include <iostream>

const size_t default_size{ 8 };

namespace flat_map {
    class FlatMap {
    private:

        struct map_element_st {
            std::string key;
            std::string value;
        };

        map_element_st* arr;
        size_t arr_size = default_size;
        size_t count = 0;

        size_t binary_search(const std::string& key);

    public:

        friend void swap(FlatMap& a1, FlatMap& a2);

        // стандартный конструктор
        FlatMap() {
            arr = new map_element_st[arr_size];
        }

        // конструктор копирования
        FlatMap(const FlatMap& other_map);

        // деструктор
        ~FlatMap() {
            delete[] arr;
        }

        // оператор присваивания
        FlatMap& operator=(const FlatMap& other_map);

        // получить количество элементов в таблице
        std::size_t size() const {
            return count;
        }

        // доступ / вставка элемента по ключу
        std::string& operator[](const std::string& key);

        // возвращает true, если запись с таким ключом присутствует в таблице
        bool contains(const std::string& key) {
            size_t i = binary_search(key);
            if ((i == count) || (arr[i].key != key)) {
                return 0;
            }

            return 1;
        }
        
        // удаление элемента по ключу, возвращает количество удаленных элементов (0 или 1)
        std::size_t erase(const std::string& key);

        // очистка таблицы, после которой size() возвращает 0, а contains() - false на любой ключ
        void clear();

        // конструктор перемещения
        FlatMap(FlatMap&& x) noexcept;

        // оператор присваивания с перемещением
        FlatMap& operator=(FlatMap&& x) noexcept;

        class Iterator;

        // Получить итератор на первый элемент
        Iterator begin() {
            return arr;
        }

        // Получить итератор на элемент, следующий за последним
        Iterator end() {
            return arr + count;
        }

        // Получить итератор на элемент по данному ключу, или на end(), если такого ключа нет.
        // В отличие от operator[] не создает записи для этого ключа, если её ещё нет
        Iterator find(const std::string& x) {
            size_t i = binary_search(x);
            if ((i == count) || (arr[i].key != x)) {
                return end();
            }
            else {
                return arr + i;
            }
        }

        class Iterator {
        private:
            map_element_st* cur;

        public:
            Iterator(map_element_st* first)
                : cur(first) { };
            Iterator()
                : cur(nullptr) { };

            Iterator& operator++ () {
                cur++;
                return *this;
            }

            Iterator& operator+= (int x) {
                cur += x;
                return *this;
            }

            Iterator operator++ (int) {
                Iterator tmp = *this;
                ++* this;
                return (tmp);
            }

            bool operator!= (const Iterator& it) {
                return cur != it.cur;
            }
            bool operator== (const Iterator& it) {
                return cur == it.cur;
            }

            map_element_st* operator->() {
                return cur;
            }
        };
    };

}