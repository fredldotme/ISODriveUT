import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

ApplicationWindow {
    id: window
    visible: true

    property bool dialogIsOpen : false
    property bool isRefreshing : false

    header: PageHeader {
        visible: !dialogIsOpen
        title: qsTr("ISODrive")

        readonly property bool hasLoadedIso :
            (isoManager.selectedISO.length > 0)
        readonly property string activeIso :
            hasLoadedIso ? isoManager.selectedISO : qsTr("none")

        subtitle: qsTr("Active: %1").arg(activeIso)

        trailingActionBar.actions: [
            Action {
                text: qsTr("Eject")
                iconName: "media-eject"
                onTriggered: {
                    isoManager.resetISO()
                    refreshList()
                }
            }
        ]
    }

    Component {
        id: dialog

        Dialog {
            id: dialogue
            title: qsTr("Authentication required")
            text: qsTr("Please enter your user PIN or password to continue:")

            Timer {
                id: enterDelayTimer
                interval: 1000
                running: false
                onTriggered: entry.text = ""
            }

            TextField {
                id: entry
                placeholderText: qsTr("PIN or password")
                echoMode: TextInput.Password
                focus: true
                enabled: !enterDelayTimer.running
            }
            Button {
                text: qsTr("Ok")
                color: UbuntuColors.green
                enabled: !enterDelayTimer.running
                onClicked: {
                    isoManager.userPassword = entry.text
                    if (isoManager.validatePassword()) {
                        refreshList()
                        PopupUtils.close(dialogue)
                        dialogIsOpen = false
                    } else {
                        enterDelayTimer.start()
                    }
                }
            }
            Button {
                text: qsTr("Cancel")
                color: UbuntuColors.red
                enabled: !enterDelayTimer.running
                onClicked: {
                    PopupUtils.close(dialogue)
                    dialogIsOpen = false
                    Qt.quit()
                }
            }
        }
    }

    Column {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        ButtonGroup {
            buttons: isoList.children
        }

        ListView {
            id: isoList
            width: parent.width
            height: parent.height
            model: fileManager.foundFiles
            clip: true

            delegate: RadioButton {
                readonly property string name : isoList.model[index].name
                readonly property string path : isoList.model[index].path
                readonly property bool isoEnabled : isoManager.selectedISO === name

                width: parent.width
                height: units.gu(8)
                text: name
                font.pixelSize: units.gu(2)
                checked: isoEnabled
                onCheckedChanged: {
                    if (checked) {
                        if (isoManager.selectedISO !== path) {
                            console.log("Selection: " + path)
                            isoManager.enableISO(path)
                        }
                    }
                    refreshList()
                }
            }

            PullToRefresh {
                refreshing: isRefreshing
                onRefresh: {
                    isRefreshing = true
                    refreshList()
                    isRefreshing = false
                }
            }
        }
    }

    Label {
        anchors.fill: parent
        anchors.margins: 16
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        text: qsTr("No .iso files found in the 'Downloads' folder. " +
                   "Download a hybrid ISO file to continue.")
        visible: isoList.count < 1
        font.pixelSize: units.gu(3)
    }

    function refreshList() {
        fileManager.refresh()
    }

    Component.onCompleted: {
        dialogIsOpen = true
        PopupUtils.open(dialog)
    }
}
