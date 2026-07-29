add_executable(optideck)

ecm_add_qml_module(optideck
        URI
        com.nikhil-prabhu.optideck
)

target_sources(optideck
        PRIVATE
        main.cpp
        core/CameraManager.h
        core/CameraManager.cpp
        core/DeviceManager.h
        core/DeviceManager.cpp
        core/HidManager.h
        core/HidManager.cpp
        core/HidScanner.h
        core/HidScanner.cpp
        core/ThemeImageProvider.h
        core/V4L2Controller.h
        core/V4L2Controller.cpp
        core/V4L2Scanner.h
        core/V4L2Scanner.cpp
)

ecm_target_qml_sources(optideck
        SOURCES
        ui/Main.qml
        ui/DashboardView.qml
        ui/HidDetailView.qml
        ui/WebcamDetailView.qml
)

target_link_libraries(optideck
        PRIVATE
        Qt6::Quick
        Qt6::Qml
        Qt6::Gui
        Qt6::QuickControls2
        Qt6::Multimedia
        Qt6::Widgets
        KF6::I18n
        KF6::IconThemes
)
