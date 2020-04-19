import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12

Window {
    id: root
    visible: true
    width: 1024
    height: 600
    color: "black"
    title: graphCore.sourceFileName
    property int highestZ: 0
    property real zoomFactor: 1.5

    Component.onCompleted: zoomFactor = graphCore.zoomFactor

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if (mouse.button === Qt.RightButton)
                contextMenu.popup()
        }
        onPressAndHold: {
            if (mouse.source === Qt.MouseEventNotSynthesized)
                contextMenu.popup()
        }

        Menu {
            id: contextMenu
            MenuItem {
                text: qsTr("Add Node...")
                onTriggered: {
                    var name = "Something"
                    graphCore.addGraphNode(name, mouseArea.mouseX, mouseArea.mouseY)
                }
            }
            MenuItem { text: qsTr("Save") }
            MenuItem { text: qsTr("Save As...") }
            MenuItem { text: qsTr("Open...") }
        }
    }

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: width * zoomFactor
        contentHeight: height * zoomFactor

        Repeater {
            model: graphCore.graphNodes
            Rectangle {
                id: graphNode
                property bool setected: false
                width: 250
                height: 300
                radius: 5
                Behavior on scale { NumberAnimation { duration: 200 } }
                color: "lightgray"
                border.color: setected ? "red" : "black"
                border.width: 5
                smooth: true
                antialiasing: true

                Component.onCompleted: {
                    x = modelData.xCoord
                    y = modelData.yCoord
                }
                onXChanged: modelData.xCoord = x
                onYChanged: modelData.yCoord = y

                GridLayout {
                    anchors.fill: parent
                    anchors.margins: 6
                    columns: 2
                    rows: 2
                    columnSpacing: 0
                    Text {
                        text: modelData.name
                        font.bold: true
                        font.pointSize: 12
                        Layout.columnSpan: 2
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                        Layout.topMargin: 4
                    }
                    Column {
                        id: inputPortColumn
                        Layout.preferredWidth: parent.width / 2
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                        onWidthChanged: console.debug("Input " + width)
                        clip: true
                        spacing: 2
                        Repeater {
                            model: modelData.inputPorts
                            RowLayout {
                                width: inputPortColumn.width
                                Rectangle {
                                    color: modelData.color
                                    radius: height / 2
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: height
                                }
                                Text { text: modelData.name; clip: true; elide: Text.ElideRight; Layout.fillWidth: true }
                            }
                        }
                    }
                    Column {
                        id: outputPortColumn
                        Layout.preferredWidth: parent.width / 2
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignRight | Qt.AlignTop
                        onWidthChanged: console.debug("Output " + width)
                        clip: true
                        spacing: 2
                        Repeater {
                            model: modelData.outputPorts
                            RowLayout {
                                width: outputPortColumn.width
                                Text { text: modelData.name; clip: true; elide: Text.ElideRight; horizontalAlignment: Qt.AlignRight; Layout.fillWidth: true }
                                Rectangle {
                                    color: modelData.color
                                    radius: height / 2
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: height
                                    Layout.alignment: Qt.AlignRight
                                }
                            }
                        }
                    }
                }

                MouseArea {
                    id: dragArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    drag.target: graphNode
                    scrollGestureEnabled: false  // 2-finger-flick gesture should pass through to the Flickable
                    onPressed: {
                        graphNode.z = ++root.highestZ;
                        if (mouse.button === Qt.RightButton)
                            nodeMenu.popup()
                        else
                            parent.setected = !parent.setected
                    }

                    Menu {
                        id: nodeMenu
                        MenuItem {
                            text: qsTr("Add Output Port...")
                            onTriggered: {
                                var name = "NewPort"
                                graphCore.addGraphNode(name, mouseArea.mouseX, mouseArea.mouseY)
                            }
                        }
                        MenuItem { text: qsTr("Add Intput Port...") }
                    }
                }
            }
        }
    }

    Rectangle {
        id: verticalScrollDecorator
        anchors.right: parent.right
        anchors.margins: 2
        color: "cyan"
        border.color: "black"
        border.width: 1
        width: 5
        radius: 2
        antialiasing: true
        height: flick.height * (flick.height / flick.contentHeight) - (width - anchors.margins) * 2
        y: (flick.contentY - flick.originY) * (flick.height / flick.contentHeight)
        NumberAnimation on opacity { id: vfade; to: 0; duration: 500 }
        onYChanged: { opacity = 1.0; scrollFadeTimer.restart() }
    }

    Rectangle {
        id: horizontalScrollDecorator
        anchors.bottom: parent.bottom
        anchors.margins: 2
        color: "cyan"
        border.color: "black"
        border.width: 1
        height: 5
        radius: 2
        antialiasing: true
        width: flick.width * (flick.width / flick.contentWidth) - (height - anchors.margins) * 2
        x: (flick.contentX - flick.originY) * (flick.width / flick.contentWidth)
        NumberAnimation on opacity { id: hfade; to: 0; duration: 500 }
        onXChanged: { opacity = 1.0; scrollFadeTimer.restart() }
    }

    Timer { id: scrollFadeTimer; interval: 1000; onTriggered: { hfade.start(); vfade.start() } }
}
