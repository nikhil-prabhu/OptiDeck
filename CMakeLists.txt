cmake_minimum_required(VERSION 4.3)
project(OptiDeck LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(ECM 6.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(ECMFindQmlModule)
include(ECMQmlModule)

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Quick
        Gui
        QuickControls2
        Multimedia
        Widgets
)
find_package(KF6 REQUIRED COMPONENTS
        Kirigami
        I18n
        QQC2DesktopStyle
        IconThemes
)


qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

ecm_find_qmlmodule(org.kde.kirigami REQUIRED)

add_subdirectory(src)

install(PROGRAMS com.nikhil-prabhu.optideck DESTINATION ${KDE_INSTALL_APPDIR})