#include "Header.h"

namespace flat_map {

    FlatMap::FlatMap(const FlatMap& other_map)
        : arr(other_map.arr_size ? new map_element_st[other_map.arr_size] : nullptr)
        , arr_size(other_map.count)
        , count(other_map.count) {
        if (arr) {
            std::copy(other_map.arr, other_map.arr + other_map.count, arr);
        }
    }

    //first variant
    /*FlatMap& FlatMap::operator=(const FlatMap& other_map) {
        if (&other_map != this)
        {
            delete[] arr;
            arr = new map_element_st[other_map.count];
            std::copy(other_map.arr, other_map.arr + other_map.count, arr);

            arr_size = other_map.count;
            count = other_map.count;
        }

        return *this;
    }*/

    void swap(FlatMap& a1, FlatMap& a2) {
        using std::swap;
        swap(a1.arr, a2.arr);
        swap(a1.count, a2.count);
        swap(a1.arr_size, a2.arr_size);

        /*map_element_st* t = a2.arr;
        a2.arr = a1.arr;
        a1.arr = t;

        size_t s = a2.arr_size;
        a2.arr_size = a1.arr_size;
        a1.arr_size = s;

        size_t c = a2.count;
        a2.count = a1.count;
        a1.count = c;*/
    }

    FlatMap& FlatMap::operator=(const FlatMap& other_map) {
        if (&other_map != this)
        {
            FlatMap tmp(other_map);
            swap(tmp, *this);
        }

        return *this;
    }

    std::string& FlatMap::operator[](const std::string& key) {
        size_t index = binary_search(key);
        if ((index == count) || (arr[index].key != key)) {
            if (count == arr_size) {
                arr_size *= 2;

                FlatMap tmp(*this);
                swap(tmp, *this);

                /*map_element_st* arr_n = new map_element_st[arr_size];

                for (size_t i = 0; i < count; i++) {
                    arr_n[i] = arr[i];
                }

                delete[] arr;
                arr = arr_n;*/
            }

            size_t ret_pos = index;

            map_element_st last = { key, "" };
            map_element_st next;
            while (index <= count) {
                if (index == count) {
                    arr[index] = last;
                }
                else {
                    next = arr[index];
                    arr[index] = last;
                    last = next;
                }

                index++;
            }

            count++;

            return arr[ret_pos].value;
        }
        else {
            return arr[index].value;
        }
    }

    size_t FlatMap::binary_search(const std::string& key) {
        if (count == 0) {
            return 0;
        }
        size_t l = 0;
        size_t r = count - 1;
        size_t mid = 0;
        bool flag = false;

        while ((l <= r && r != SIZE_MAX) && (flag != true)) {
            mid = (r + l) / 2;

            if (arr[mid].key == key) {
                flag = true;
            }
            else if (arr[mid].key > key) {
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

    std::size_t FlatMap::erase(const std::string& key) {
        size_t i = binary_search(key);
        if ((i == count) || (arr[i].key != key)) {
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

    void FlatMap::clear() {
        count = 0;
        arr_size = default_size;
        FlatMap tmp(*this);
        swap(tmp, *this);
        //map_element_st* arr_n = new map_element_st[arr_size];
        //delete[] arr;
        //arr = arr_n;
    }

    FlatMap::FlatMap(FlatMap&& x) noexcept {
        arr = x.arr;
        arr_size = x.count;
        count = x.count;
        x.arr = nullptr;
        x.count = 0;
    }

    FlatMap& FlatMap::operator=(FlatMap&& x) noexcept {
        if (&x != this)
        {
            delete[] arr;
            arr = x.arr;
            arr_size = x.count;
            count = x.count;
            x.arr = nullptr;
            x.count = 0;
        }

        return *this;
    }
}