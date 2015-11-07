cmake_minimum_required(VERSION 2.8)      # Проверка версии CMake.
                                         # Если версия установленой программы
                                         # старее указаной, произайдёт аварийный выход.

project(testing_thread_pool)             # Название проекта
set(CMAKE_C_COMPILER  "/home/ikulagin/workspace/tm_exampls/test_hash/bin/gcc_tm_prof")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -std=gnu1x -Wall -I${CMAKE_CURRENT_SOURCE_DIR}")

set(SOURCE_TM_EXE 1_tm.c)                      # Установка переменной со списком исходников для исполняемого файла
set(SOURCE_MUTEX_EXE
  mutex/1_mutex.c)             # Установка переменной со списком исходников для исполняемого файла

set(SOURCE_THPOOL_LIB thread_pool.c)        # Тоже самое, но для библиотеки
set(SOURCE_TMHASHT_LIB hashtable.c)
set(SOURCE_MUTEX_HASHT_LIB 
  mutex/hashtable_mutex.c)
set(SOURCE_TMLIST_LIB list.c)
set(SOURCE_MUTEXLIST_LIB
  mutex/list_mutex.c)

link_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(thread_pool STATIC ${SOURCE_THPOOL_LIB})    # Создание статической библиотеки с именем foo
add_library(tm_hashtable STATIC ${SOURCE_TMHASHT_LIB})    # Создание статической библиотеки с именем foo
add_library(mutex_hashtable STATIC ${SOURCE_MUTEX_HASHT_LIB})    # Создание статической библиотеки с именем foo
add_library(tm_list STATIC ${SOURCE_TMLIST_LIB})    # Создание статической библиотеки с именем foo
add_library(mutex_list STATIC ${SOURCE_MUTEXLIST_LIB})    # Создание статической библиотеки с именем foo

add_executable(1_mutex.out ${SOURCE_MUTEX_EXE}) # Создает исполняемый файл с именем main
add_executable(1_tm.out ${SOURCE_TM_EXE})       # Создает исполняемый файл с именем main

set_target_properties(1_tm.out PROPERTIES LINK_FLAGS -fgnu-tm COMPILE_FLAGS -fgnu-tm)
set_source_files_properties(${SOURCE_TMLIST_LIB} PROPERTIES COMPILE_FLAGS -fgnu-tm)
set_source_files_properties(${SOURCE_TMHASHT_LIB} PROPERTIES COMPILE_FLAGS -fgnu-tm)
target_link_libraries(1_mutex.out thread_pool mutex_hashtable mutex_list) # Линковка программы с библиотекой
target_link_libraries(1_tm.out thread_pool tm_hashtable tm_list) # Линковка программы с библиотекой
