cmake_minimum_required(VERSION 4.3)
project(OptiDeck LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

qt_add_executable(appOptiDeck
        src/main.cpp
        src/core/V4L2Scanner.cpp
        src/core/V4L2Scanner.h
)

qt_add_qml_module(appOptiDeck
        URI OptiDeck
        VERSION 0.1
        RESOURCE_PREFIX "/qt/qml"
        QML_FILES
            src/ui/Main.qml
)

target_include_directories(appOptiDeck PRIVATE
        src
)

target_link_libraries(appOptiDeck PRIVATE
        Qt6::Quick
)

set_target_properties(appOptiDeck PROPERTIES OUTPUT_NAME "optideck")
