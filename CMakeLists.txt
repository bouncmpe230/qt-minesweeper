cmake_minimum_required(VERSION 3.21)

project(MinesweeperQt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

include(GNUInstallDirs)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(minesweeper
    src/cellbutton.cpp
    src/cellbutton.h
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/minesweepergame.cpp
    src/minesweepergame.h
    src/tutorialdialog.cpp
    src/tutorialdialog.h
)

target_link_libraries(minesweeper PRIVATE Qt6::Widgets)

install(TARGETS minesweeper
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

