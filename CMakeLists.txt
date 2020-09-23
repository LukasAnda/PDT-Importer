cmake_minimum_required(VERSION 3.10)
project(importer C)

set(CMAKE_C_STANDARD 99)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${PROJECT_SOURCE_DIR}/libs/pgsql/include -L/${PROJECT_SOURCE_DIR}/libs/pgsql/lib")
##set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${PROJECT_SOURCE_DIR}/libs/pgsql/include")
##set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${PROJECT_SOURCE_DIR}/libs/pgsql/lib")

# Next line is not required, without it given path will be checked too.
set(PostgreSQL_ROOT "${PROJECT_SOURCE_DIR}/libs/pgsql")
find_package(PostgreSQL REQUIRED)

include_directories (${PostgreSQL_INCLUDE_DIRS})
link_directories(${PostgreSQL_LIBRARY_DIRS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SOURCES main.c)
add_executable(importer ${SOURCES})
target_link_libraries(importer PRIVATE ${PostgreSQL_LIBRARIES})
target_link_libraries(importer PRIVATE Threads::Threads)

set(SOURCE_FILES CMakeLists.txt main.c)