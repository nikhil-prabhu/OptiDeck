import QtQuick
import org.kde.kirigami as Kirigami
import QtMultimedia

Kirigami.ApplicationWindow {
    id: root
    width: 1000
    height: 700
    title: i18nc("@title:window", "OptiDeck")

    MediaDevices {
        id: appMediaDevices
    }

    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.globalToolBar.showNavigationButtons: Kirigami.ApplicationHeaderStyle.ShowBackButton
    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn

    pageStack.initialPage: dashboardViewComponent
    pageStack.popHiddenPages: true

    Component {
        id: dashboardViewComponent
        DashboardView {
            onOpenWebcamDetail: (deviceData) => {
                deviceData.videoInputs = appMediaDevices.videoInputs
                root.pageStack.push(webcamDetailViewComponent, deviceData)
            }
            onOpenHidDetail: (deviceData) => {
                root.pageStack.push(hidDetailViewComponent, deviceData)
            }
        }
    }

    Component {
        id: webcamDetailViewComponent
        WebcamDetailView {
        }
    }

    Component {
        id: hidDetailViewComponent
        HidDetailView {
        }
    }
}
