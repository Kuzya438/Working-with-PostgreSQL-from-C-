cmake_minimum_required(VERSION 3.20)

project(MyProject)

set(PostgreSQL_ROOT "C:/Program Files/PostgreSQL/18")

add_executable(MyProject main.cpp Client.h Client.cpp)

add_subdirectory(libpqxx-8.0.2 libpqxx-build)

target_compile_features(MyProject PRIVATE cxx_std_20)
target_link_libraries(MyProject PRIVATE pqxx)