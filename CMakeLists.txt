cmake_minimum_required(VERSION 4.3)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(untitled
        main.cpp
        src/Console.cpp
        src/MasterManagement.cpp
        src/Table.cpp
        src/Metadata.cpp
        src/Constraints.cpp
        src/DataBase.cpp
)