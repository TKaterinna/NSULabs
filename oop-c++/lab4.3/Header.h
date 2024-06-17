#pragma once
#include <iostream>
#include <memory>

template <typename T>
class CustomAllocator : public std::allocator<T> {
private:
    T* allocate_pool(size_t count) {
        return static_cast<T*>(::operator new(count * sizeof(T)));
    }
    void deallocate_pool(T* pool) {
        ::operator delete(pool);
    }

    size_t capacity = 64;
    T* memory_pool;
public:
    CustomAllocator(size_t _capacity) : capacity(_capacity) {
        memory_pool = allocate_pool(capacity);
    }
    CustomAllocator() {
        memory_pool = allocate_pool(capacity);
    }

    ~CustomAllocator() {
        deallocate_pool(memory_pool);
    }

    T* allocate(size_t n) {
        if (n > capacity) {
            throw std::bad_alloc();
        }
        return memory_pool;
    }

    void deallocate(T* p, size_t n) {
        n+=1;
        n-=1;
        p+=1;
        p-=1;
    };
};

struct A {
        int x;
        int y;
};

template <typename T>
T null() {
    return 0;
}
template <>
std::string null() {
    return " ";
}

const size_t default_size{ 8 };

template <typename K, typename V>
struct element_st {
    K key;
    V value;
};

namespace flat_map {
    template <typename K, typename V, class Compare = std::less<K>, class Equal = std::equal_to<K>, class Allocator = std::allocator<std::pair<K,V>>>
    class FlatMap {
    private:
        using map_element_st = std::pair<K, V>;
        using allocator_traits = std::allocator_traits<Allocator>;
        //std::allocator_traits<Allocator> allocator_traits;
        Allocator _a;

        map_element_st* arr;
        size_t arr_size = default_size;
        size_t count = 0;

        size_t binary_search(const K& key) {
            if (count == 0) {
                return 0;
            }
            size_t l = 0;
            size_t r = count - 1;
            size_t mid = 0;
            bool flag = false;

            while ((l <= r && r != SIZE_MAX) && (flag != true)) {
                mid = (r + l) / 2;

                if (Equal{}(arr[mid].first, key)) {
                    flag = true;
                }
                else if (Compare{}(key, arr[mid].first)/*arr[mid].key > key*/) {
                    r = mid - 1;
                }
                else {
                    l = mid + 1;
                }
            }

            if (flag) {
                return mid;
            }
            else {
                return l;
            }
        }

    public:

        friend void swapp(FlatMap& a1, FlatMap& a2) {
            //using std::swap;
            std::swap(a1.arr, a2.arr);
            std::swap(a1.count, a2.count);
            std::swap(a1.arr_size, a2.arr_size);
        }

        // стандартный конструктор
        FlatMap() : arr(allocator_traits::allocate(_a, default_size)) {};

        //конструктор копирования
        FlatMap(const FlatMap& other_map) {
            arr = other_map.arr_size ? allocator_traits::allocate(_a, other_map.arr_size)/*new map_element_st[other_map.arr_size]*/: nullptr;
            arr_size = other_map.count;
            count = other_map.count;
            if (arr) {
                std::uninitialized_copy(other_map.arr, other_map.arr + other_map.count, arr);
            }
        }

        // деструктор
        ~FlatMap() {
            allocator_traits::deallocate(_a, arr, arr_size);
            //delete[] arr;
        }
        // оператор присваивания
        FlatMap& operator=(const FlatMap& other_map) {
            if (&other_map != this)
            {
                FlatMap tmp(other_map);
                swapp(tmp, *this);
            }

            return *this;
        }
        // получить количество элементов в таблице
        std::size_t size() const {
            return count;
        }
        // доступ / вставка элемента по ключу
        V& operator[](const K& key) {
            size_t index = binary_search(key);
            if ((index == count) || (!Equal{}(arr[index].first, key))) {
                if (count == arr_size) {
                    arr_size *= 2;

                    FlatMap<K, V, Compare, Equal, Allocator> tmp(*this);
                    swapp(tmp, *this);
                }

                size_t ret_pos = index;
                V n = null<V>();
                //map_element_st last = { key, n };
                map_element_st last;
                K c_key = key;
                map_element_st next;

                while (index <= count) {
                    if (index == count) {
                        allocator_traits::construct(_a, arr + index, std::move(c_key), std::move(n));
                        //arr[index].key = last.key;
                    }
                    else {
                        next = arr[index];
                        if (index == ret_pos) {
                            allocator_traits::construct(_a, arr + index, std::move(c_key), std::move(n));
                        }
                        else {
                            arr[index] = last;
                        }
                        last = next;
                    }
                    index++;
                }
                count++;
                return arr[ret_pos].second;
            }
            else {
                return arr[index].second;
            }
        }

        // возвращает true, если запись с таким ключом присутствует в таблице
        bool contains(const K& key) {
            size_t i = binary_search(key);
            if ((i == count) || (!Equal{}(arr[i].first, key))) {
                return 0;
            }
            return 1;
        }
        // удаление элемента по ключу, возвращает количество удаленных элементов (0 или 1)
        std::size_t erase(const K& key) {
            size_t i = binary_search(key);
            if ((i == count) || (!Equal{}(arr[i].first, key))) {
                return 0;
            }
            else {
                for (i; i < count - 1; i++) {
                    arr[i] = arr[i + 1];
                }
                count--;
                return 1;
            }

        }
        // очистка таблицы, после которой size() возвращает 0, а contains() - false на любой ключ
        void clear() {
            count = 0;
            arr_size = default_size;
            FlatMap<K, V, Compare, Equal, Allocator> tmp(*this);
            swapp(tmp, *this);
        }
        // конструктор перемещения
        FlatMap(FlatMap&& x) noexcept {
            arr = x.arr;
            arr_size = x.count;
            count = x.count;
            x.arr = nullptr;
            x.count = 0;
        }
        // оператор присваивания с перемещением
        FlatMap& operator=(FlatMap&& x) noexcept {
            if (&x != this)
            {
                //delete[] arr;
                allocator_traits::deallocate(_a, arr, arr_size);
                arr = x.arr;
                arr_size = x.count;
                count = x.count;
                x.arr = nullptr;
                x.count = 0;
            }
            return *this;
        }

        class Iterator;
        Iterator begin() {
            return arr;
        }
        Iterator end() {
            return arr + count;
        }
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
                ++*this;
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