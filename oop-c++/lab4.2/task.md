# Задача 4. Шаблоны (templates) и элементы метапрограммирования

## Подзадача 2. Парсер CSV

Формат CSV: табличные данные могут быть представлены как текстовый файл с разделителем `\n` между строками и символом `,` для разделения ячеек внутри строки. Считаем, что данные символы не встречаются внутри данных.

Напишите класс, делающий возможным следующую потоковую работу с CSV:
```C++
int main()
{
    ifstream csv_stream("test.csv");    
    CsvParser<int, std::string, double> parser(csv_stream, 0 /*skip first lines count*/);
    for (std::tuple<int, std::string, double> rs : parser) {
        std::cout << rs << "\n";
    }
}
```

Потоковая обработка подразумевает lazy (ленивое) чтение строк, то есть вы читаете строку только тогда, когда она реально нужна. Таким образом необходимо реализовать [InputIterator](https://en.cppreference.com/w/cpp/named_req/InputIterator) для чтения данных в CSV файле. 

При сдаче продемонстрировать:
1. Чтение нескольких валидных файлов и печать их содержимого в консоль.
2. Чтение из `std::cin`.
3. Чтение нескольких ошибочных файлов и/или не подходящих по типам.

**Дополнительно:**
1. Реализуйте проверку на этапе компиляции, что все шаблонные параметры парсера могут быть получены из строк (то есть, вы можете строку распарсить в этот тип). Если это не так, выведите ваше собственное понятное сообщение об ошибке.
2. Добавьте в конец кортежа, который возвращает итератор, параметр типа `std::vector<std::string>`, куда будут записаны все остальные колонки, если их в файле больше, чем шаблонных параметров парсера. То есть для примера выше итератор должен возвращать тип `std::tuple<int, std::string, double, std::vector<std::string>>`.
3. Выделите логику приведения строки из CSV-файла к нужному типу в отдельную **стратегию**. Для этого сделайте так, чтобы первым шаблонным параметром CsvParser можно было указать шаблонный функтор для приведения строки к нужному типу. Сделайте реализацию этого функтора, которая использует `operator<<`, и другую реализацию, которая строку возвращает как есть, для числовых типов использует функции `std::sto*` (например, `std::stoi` для `int`), а остальные типы по умолчанию не поддерживает (при попытке их использовать возникает ошибка компиляции).
