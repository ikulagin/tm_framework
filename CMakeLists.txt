cmake_minimum_required(VERSION 2.8)      # Проверка версии CMake.
                                         # Если версия установленой программы
                                         # старее указаной, произайдёт аварийный выход.

project(testing_thread_pool)             # Название проекта
#set(CMAKE_C_COMPILER  "/home/ikulagin/workspace/gcc-5.0/bin/gcc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -std=gnu1x -Wall")

set(SOURCE_EXE 1.c)                      # Установка переменной со списком исходников для исполняемого файла

set(SOURCE_THPOOL_LIB thread_pool.c)            # Тоже самое, но для библиотеки
set(SOURCE_TMHASHT_LIB hashtable.c)

add_library(thread_pool STATIC ${SOURCE_THPOOL_LIB})    # Создание статической библиотеки с именем foo
add_library(tm_hashtable STATIC ${SOURCE_TMHASHT_LIB})    # Создание статической библиотеки с именем foo

add_executable(1.out ${SOURCE_EXE})      # Создает исполняемый файл с именем main

target_link_libraries(1.out thread_pool tm_hashtable) # Линковка программы с библиотекой
