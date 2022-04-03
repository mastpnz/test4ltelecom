import QtQuick 2.12
import QtQuick.Window 2.12
import QtQml 2.12
import "./"
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.12

Window {
    id: root
    width: 240
    height: 120
    visible: true
    title: qsTr("LTelecom Test Client")

    property string analysefile: ""
    property bool srv_connected: false

    Connections {
        target: m_client
        onConnected: srv_connected = true
        onDisconnected: srv_connected = false
        onAnalyseComplete: {
            var component = Qt.createComponent("ReportWindow.qml")
            var window    = component.createObject(root)
            window.reportName = name
            window.reportData = data
            window.show()
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        onAccepted: {
            var path = fileDialog.fileUrl.toString();
            path = path.replace(/^(file:\/{2})/,"");
            m_client.openAndSendFile(decodeURIComponent(path))
            fileDialog.close()
        }
        onRejected: {
            fileDialog.close()
        }
        Component.onCompleted: visible = false
    }

    Column {
        spacing: 3
        topPadding: 3
        Row {
            spacing: 3
            padding: 3
            TextField {
                id: addr_input
                maximumLength: 16
                validator: RegExpValidator {
                                regExp:  /^((?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\.){0,3}(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$/
                            }
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                height: 30
                width: root.width - btn_connect.width - parent.padding*3
                placeholderText: qsTr("Server IP address")
                visible: !srv_connected
            }
            Rectangle {
                color: "blue"
                height: 30
                width: root.width - btn_connect.width - parent.padding*3
                visible: srv_connected
                Text {
                    color: "white"
                    anchors.fill: parent
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: "Connected"
                }
            }
            Button {
                id: btn_connect
                width: 80
                height: 30
                text: srv_connected ? "disconnect" : "connect"
                onClicked: {
                        if(srv_connected)
                            m_client.disconnectFromHost();
                        else
                            m_client.connectToServer(addr_input.text);
                }
            }
        }
        Row {
            spacing: 3
            padding: 3
            Button {
                id: btn_choicefile
                width: 100
                height: 30
                text: "send file"
                visible: srv_connected
                onClicked: {
                    fileDialog.open()
                }
            }
        }
        Row {
            spacing: 3
            padding: 3
            Button {
                id: btn_getsrvstat
                width: 100
                height: 30
                text: "get stat"
                visible: srv_connected
                onClicked: {
                    m_client.sendCommand();
                }
            }

        }
    }

}
