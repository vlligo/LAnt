cmake_minimum_required(VERSION 3.25)
project(LAnt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(LAnt main.cpp paint.h paint.cpp)
target_link_libraries(LAnt
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        )

