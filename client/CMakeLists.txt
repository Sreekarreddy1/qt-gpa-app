cmake_minimum_required(VERSION 3.16)
project(GpaClient)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)          # lets Qt process Q_OBJECT classes

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

add_executable(gpa_client main.cpp mainwindow.h mainwindow.cpp)
target_link_libraries(gpa_client PRIVATE Qt6::Widgets Qt6::Network)