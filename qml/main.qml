import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Qt.labs.settings 1.0
import QtQml 2.2
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3
import Lomiri.Components.Themes 1.3

ApplicationWindow {
    id: window
    visible: true

    property bool dialogIsOpen : false
    property bool settingsDialogOpen : false
    property bool isRefreshing : false

    readonly property bool hasLoadedIso :
        (isoManager.selectedISO.length > 0)
    readonly property string activeIso :
        hasLoadedIso ? isoManager.selectedISO : qsTr("none")


    header: PageHeader {
        visible: !(dialogIsOpen || settingsDialogOpen)
        title: qsTr("ISODrive")

        subtitle: qsTr("Active: %1").arg(activeIso)

        trailingActionBar.actions: [
            Action {
                text: qsTr("Eject")
                iconName: "media-eject"
                enabled: hasLoadedIso
                onTriggered: {
                    isoManager.resetISO()
                    refreshList()
                }
            },
            Action {
                text: qsTr("Settings")
                iconName: "settings"
                enabled: hasSettings && !hasLoadedIso && !settingsDialogOpen
                onTriggered: {
                    PopupUtils.open(settingsDialog)
                    settingsDialogOpen = true
                }
            }
        ]
    }

    Settings {
        id: settings
        property bool enableMtp : false
        property bool includeImgFiles : false
    }

    // First start password entry
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
                color: theme.palette.normal.positive

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
                enabled: !enterDelayTimer.running
                onClicked: {
                    PopupUtils.close(dialogue)
                    dialogIsOpen = false
                    Qt.quit()
                }
            }
        }
    }

    // Settings dialog
    Component {
        id: settingsDialog

        Dialog {
            id: settingsDialogue
            title: qsTr("Settings")

            Column {
                spacing: units.gu(2)
                
                CheckBox {
                    id: mtpCheckBox
                    text: qsTr("Enable MTP while using ISODrive")
                    checked: settings.enableMtp
                    onCheckedChanged: {
                        settings.enableMtp = checked
                    }
                }

                CheckBox {
                    id: imgCheckBox
                    text: qsTr("Include .img files (USB drive images)")
                    checked: settings.includeImgFiles
                    onCheckedChanged: {
                        settings.includeImgFiles = checked
                        fileManager.setIncludeImgFiles(checked)
                    }
                }

                Label {
                    text: qsTr("Custom search paths:")
                    font.bold: true
                }

                Button {
                    text: qsTr("Add /documents/iso path")
                    onClicked: {
                        fileManager.addCustomPath("/documents/iso")
                    }
                }

                Button {
                    text: qsTr("Add /documents/flashdrive path")
                    onClicked: {
                        fileManager.addCustomPath("/documents/flashdrive")
                    }
                }

                Button {
                    text: qsTr("Add custom path...")
                    onClicked: {
                        // For now, add a default path - could be extended with input dialog
                        fileManager.addCustomPath(QStandardPaths.standardLocations(QStandardPaths.DocumentsLocation)[0] + "/custom")
                    }
                }
            }

            Button {
                text: qsTr("Ok")
                color: theme.palette.normal.positive

                onClicked: {
                    PopupUtils.close(settingsDialogue)
                    settingsDialogOpen = false
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
            model: fileManager.foundFiles
            clip: true

            delegate: ListItem {
                id: listItem
                readonly property string name : isoList.model[index].name
                readonly property string path : isoList.model[index].path
                readonly property string type : isoList.model[index].type
                readonly property bool isoEnabled : isoManager.selectedISO === name

                width: parent.width
                height: units.gu(8)

                RadioButton {
                    text: listItem.name + (listItem.type ? " (." + listItem.type + ")" : "")
                    font.pixelSize: units.gu(2)
                    checked: listItem.isoEnabled
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16
                    onCheckedChanged: {
                        if (checked) {
                            if (isoManager.selectedISO !== listItem.path) {
                                console.log("Selection: " + listItem.path)
                                isoManager.enableISO(listItem.path, settings.enableMtp)
                            }
                        }
                        refreshList()
                    }
                }

                leadingActions: ListItemActions {
                    actions: [
                        Action {
                            iconName: "delete"
                            enabled: !listItem.isoEnabled
                            onTriggered: {
                                fileManager.removeFile(listItem.path)
                                isRefreshing = true
                                refreshList()
                                isRefreshing = false
                            }
                        }
                    ]
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
        text: qsTr("No .iso or .img files found. " +
                   "Download a hybrid ISO file or place .img files in your search paths to continue.")
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
