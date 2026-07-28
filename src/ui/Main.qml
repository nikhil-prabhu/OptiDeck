import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 1000
    height: 700
    visible: true
    title: qsTr("OptiDeck")

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: dashboardViewComponent
    }

    Component {
        id: dashboardViewComponent
        DashboardView {
            onOpenWebcamDetail: (deviceData) => {
                stackView.push(webcamDetailViewComponent, deviceData)
            }
            onOpenHidDetail: (deviceData) => {
                stackView.push(hidDetailViewComponent, deviceData)
            }
        }
    }

    Component {
        id: webcamDetailViewComponent
        WebcamDetailView {
            onGoBack: stackView.pop()
        }
    }

    Component {
        id: hidDetailViewComponent
        HidDetailView {
            onGoBack: stackView.pop()
        }
    }
}