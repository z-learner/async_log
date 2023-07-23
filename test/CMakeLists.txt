cmake_minimum_required(VERSION 3.10)

PROJECT(async_log_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(../include)

find_package(fmt)


add_executable(${PROJECT_NAME} test_log.cc)
target_link_libraries(${PROJECT_NAME} fmt::fmt pthread) 