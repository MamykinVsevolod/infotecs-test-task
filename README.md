# Тестовое задание для стажера на позицию «Программист на языке C++»

Этот проект содержит две программы, которые обмениваются данными через TCP сокеты. Программа №1 принимает ввод от пользователя, обрабатывает его и отправляет Программе №2, которая анализирует полученные данные.

## Структура проекта

```plaintext
project/
├── CMakeLists.txt       # Основной файл конфигурации CMake для всего проекта
├── program1/            # Директория с исходным кодом Программы 1
│   ├── CMakeLists.txt   # Файл конфигурации CMake для Программы 1
│   └── main.cpp         # Исходный код Программы 1
└── program2/            # Директория с исходным кодом Программы 2
    ├── CMakeLists.txt   # Файл конфигурации CMake для Программы 2       
    └── main.cpp         # Исходный код Программы 2
```
## Файлы CMake для сборки
#### Основной CMakeLists.txt
```
cmake_minimum_required(VERSION 3.10)

project(Project)

add_subdirectory(program1)
add_subdirectory(program2)
```
#### Файл CMakeLists.txt для `program1`
```
cmake_minimum_required(VERSION 3.10)

project(Program1)

set(CMAKE_CXX_STANDARD 11)

add_executable(program1 main.cpp)

target_link_libraries(program1 pthread)

```
#### Файл CMakeLists.txt для `program2`
```
cmake_minimum_required(VERSION 3.10)

project(Program2)

set(CMAKE_CXX_STANDARD 11)

add_executable(program2 main.cpp)

target_link_libraries(program2 pthread)

```
## Сборка проекта 
1. Откройте терминал и перейдите в папку проекта
2. Создайте директорию для файлов сборки:
    ```sh
    mkdir build
    cd build
    ```

3. Запустите CMake для генерации файлов системы сборки:
    ```sh
    cmake ..
    ```

4. Скомпилируйте проект:
    ```sh
    make
    ```

Это создаст два исполняемых файла: `program1` и `program2`.

## Запуск программ

1. Откройте два окна терминала.

2. В первом терминале перейдите в директорию `build` и запустите `program2`:
    ```sh
    ./program2/program2
    ```

3. Во втором терминале перейдите в директорию `build` и запустите `program1`:
    ```sh
    ./program1/program1
    ```
## Пример ввода в консоли:
#### Для программы №1:
```sh
Enter a string of digits (up to 64 characters): 1234567890
Processed data: 97531KBKBKBKB
Sum of numerical values: 25
Data sent successfully.
```
#### Для программы №1:
```sh
Waiting for connection...
Connection established.
Received sum: 25
Error: The received sum does not meet the criteria.
```
