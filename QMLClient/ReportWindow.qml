import QtQuick 2.12
import QtQuick.Window 2.12
import QtQml 2.12
import "./"
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.12

    Window {
        id: show_report
        width: 240
        height: 400
        property string reportName: ""
        title: reportName
        property var reportData: ""
        ScrollView {
            anchors.fill: parent
            Column {
                anchors.fill: parent
                Repeater {
                    model: reportData.length
                    Row {
                        width: parent.width
                        spacing: 2
                        Label {
                            width: show_report.width / 3
                            height: 20
                            background: Rectangle {
                                   anchors.fill: parent
                                   color: index % 2 ? "#A7B9E0" : "#BAA6ED"
                            }
                            text: reportData[index]["Key"]
                        }
                        Label {
                            width: show_report.width / 3 * 2
                            height: 20
                            background: Rectangle {
                                   anchors.fill: parent
                                   color: index % 2 ? "#ADDAF7" : "#ADAFF7"
                            }
                            text: reportData[index]["Data"]
                        }
                    }
                }
            }
        }
    }
