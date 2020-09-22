cmake_minimum_required(VERSION 3.10)
project(importer C)

set(CMAKE_C_STANDARD 99)

# Next line is not required, without it given path will be checked too.
set(PostgreSQL_ROOT "${PROJECT_SOURCE_DIR}/libs/pgsql")
find_package(PostgreSQL REQUIRED)

include_directories (${PostgreSQL_INCLUDE_DIRS})
link_directories(${PostgreSQL_LIBRARY_DIRS})

set(SOURCES main.c)
add_executable(importer ${SOURCES})
target_link_libraries(importer ${PostgreSQL_LIBRARIES})

set(SOURCE_FILES CMakeLists.txt main.c)