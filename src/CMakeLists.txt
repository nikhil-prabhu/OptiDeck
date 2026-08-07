add_executable(optideck)

ecm_add_qml_module(optideck
        URI
        com.nikhil-prabhu.optideck
)

target_sources(optideck
        PRIVATE
        main.cpp
        core/DeviceManager.h
        core/DeviceManager.cpp
        core/HidppProtocol.h
        core/LogitechIds.h

        core/webcam/CameraManager.h
        core/webcam/CameraManager.cpp
        core/webcam/v4l2/V4L2Controller.h
        core/webcam/v4l2/V4L2Controller.cpp
        core/webcam/v4l2/V4L2Scanner.h
        core/webcam/v4l2/V4L2Scanner.cpp

        core/hid/HidManager.h
        core/hid/HidManager.cpp
        core/hid/HidScanner.h
        core/hid/HidScanner.cpp
        core/hid/HidppReceiver.h
        core/hid/HidppReceiver.cpp
        core/hid/HidppDiagnostics.h

        core/hid/receivers/ReceiverProtocolProbe.h
        core/hid/receivers/ReceiverProbeRegistry.h
        core/hid/receivers/ReceiverProbeRegistry.cpp
        core/hid/receivers/BoltProtocolProbe.h
        core/hid/receivers/BoltProtocolProbe.cpp
        core/hid/receivers/UnifyingProtocolProbe.h
        core/hid/receivers/UnifyingProtocolProbe.cpp
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
