import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

ApplicationWindow {
    id: window
    visible: true

    property bool dialogIsOpen : false

    header: ToolBar {
        visible: !dialogIsOpen
        RowLayout {
            anchors.fill: parent
            Label {
                readonly property string activeIso :
                    isoManager.selectedISO.length > 0 ?
                        isoManager.selectedISO : "none"
                text: "Active: %1".arg(activeIso)
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }
            ToolButton {
                text: "Eject"
                onClicked: {
                    isoManager.resetISO()
                    refreshList()
                }
            }
            ToolButton {
                text: "Refresh"
                onClicked: refreshList()
            }
        }
    }
    Component {
        id: dialog

        Dialog {
            id: dialogue
            title: "Authentication"
            text: "Please enter your user PIN or password to continue:"
            TextField {
                id: entry
                placeholderText: "PIN or password"
                echoMode: TextInput.Password
                focus: true
            }
            Button {
                text: "Ok"
                color: UbuntuColors.orange
                onClicked: {
                    isoManager.userPassword = entry.text
                    if (isoManager.validatePassword()) {
                        refreshList()
                        PopupUtils.close(dialogue)
                        dialogIsOpen = false
                    } else {
                        entry.text = ""
                    }
                }
            }
            Button {
                text: "Cancel"
                color: UbuntuColors.grey
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

        ButtonGroup {
            buttons: isoList.children
        }

        ListView {
            id: isoList
            width: parent.width
            height: parent.height
            contentHeight: childrenRect.height
            model: fileManager.foundFiles

            delegate: RadioButton {
                readonly property string name : isoList.model[index].name
                readonly property string path : isoList.model[index].path
                readonly property bool isoEnabled : isoManager.selectedISO == name

                width: parent.width
                height: 32
                text: name
                checked: isoEnabled
                onCheckedChanged: {
                    // Eject if this ISO is enabled
                    if (checked) {
                        if (isoManager.selectedISO != path) {
                            console.log("Selection: " + path)
                            isoManager.enableISO(path)
                        }
                    }
                    refreshList()
                }
            }
        }
    }

    Label {
        anchors.fill: parent
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        wrapMode: Label.WrapAtWordBoundaryOrAnywhere
        text: qsTr("No .iso files found in the 'Downloads' folder. " +
                   "Download a hybrid ISO file to continue.")
        visible: isoList.count < 1
    }

    function refreshList() {
        fileManager.refresh()
    }

    Component.onCompleted: {
        dialogIsOpen = true
        PopupUtils.open(dialog)
    }
}
