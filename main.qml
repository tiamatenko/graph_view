import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3

Window {
    id: root
    visible: true
    width: 1024
    height: 600
    color: "black"
    title: graphCore.sourceFileName === undefined ? qsTr("<EMPTY>") : graphCore.sourceFileName
    property int highestZ: 0
    property real zoomFactor: graphCore.zoomFactor
    property var portCoords: ({})
    property var selectedNodes: ({})

    function select(node, exclusive) {
        if (exclusive) {
            for (var nodeName in selectedNodes) {
                if (nodeName !== node.name) {
                    var n = selectedNodes[nodeName]
                    n.border.color = "black"
                }
            }
            selectedNodes = {}
        }
        selectedNodes[node.name] = node
        node.border.color = "red"
    }
    function deselect(node) {
        delete selectedNodes[node.name]
        node.border.color = "black"
    }

    function saveAs() {
        fileDialog.selectExisting = false
        fileDialog.open()
    }
    function open() {
        fileDialog.selectExisting = true
        fileDialog.open()
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        nameFilters: [ "JSON files (*.JSON *.json)", "All files (*)"]
        onAccepted: {
            if (selectExisting)
                graphCore.load(fileDialog.fileUrl)
            else
                graphCore.saveAs(fileDialog.fileUrl)
        }
    }

    MessageDialog {
        id: errorBox
        title: "Error occurred"
        icon: StandardIcon.Warning
    }

    signal graphChanged()

    function updateConnections() {
        if (active) {
            graphChanged()
            canvas.requestPaint()
        }
        else {
            portCoords = {}
        }
    }

    function clearConnections() {
        portCoords = {}
        updateConnections()
    }

    function showError(error) {
        errorBox.title = error
        errorBox.open()
    }

    onActiveChanged: updateConnections()
    onZoomFactorChanged: graphCore.zoomFactor = zoomFactor

    Component.onCompleted: {
        graphCore.errorOccurred.connect(showError)
        graphCore.graphConnectionsChanged.connect(clearConnections)
    }

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: width * zoomFactor
        contentHeight: height * zoomFactor

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
            onWheel: {
                if (wheel.modifiers & Qt.ControlModifier) {
                    zoomFactor += zoomFactor * wheel.angleDelta.y / 1200
                    wheel.accepted = true
                } else {
                    wheel.accepted = false
                }
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
                MenuItem {
                    text: qsTr("Save")
                    onTriggered: {
                        if (graphCore.sourceFileName.length > 0)
                            graphCore.save()
                        else
                            saveAs()
                    }
                }
                MenuItem {
                    text: qsTr("Save As...")
                    onTriggered: saveAs()
                }
                MenuItem {
                    text: qsTr("Open...")
                    onTriggered: open()
                }
                MenuItem {
                    text: qsTr("Clear")
                    onTriggered: graphCore.clear()
                }
            }
        }

        Repeater {
            model: graphCore.graphNodes
            Rectangle {
                id: graphNode
                property string name: modelData.name
                width: 250
                height: 300
                radius: 5
                scale: zoomFactor
                Behavior on scale { NumberAnimation { duration: 200 } }
                color: "lightgray"
                border.color: "black"
                border.width: 5
                smooth: true
                antialiasing: true

                Component.onCompleted: {
                    x = modelData.xCoord
                    y = modelData.yCoord
                }
                onXChanged: { modelData.xCoord = x; updateConnections() }
                onYChanged: { modelData.yCoord = y; updateConnections() }
                onScaleChanged: updateConnections()

                GridLayout {
                    anchors.fill: parent
                    anchors.margins: 6
                    columns: 2
                    rows: 2
                    columnSpacing: 0
                    Text {
                        text: graphNode.name
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
                        clip: true
                        spacing: 2
                        Repeater {
                            model: modelData.inputPorts
                            RowLayout {
                                width: inputPortColumn.width
                                DropArea {
                                    id: inDropArea
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: height
                                    keys: [ "out_" + modelData.color ]
                                    onDropped: {
                                        graphCore.addGraphConnection(draggedPoint.nodeName, draggedPoint.portName,
                                                                     graphNode.name, modelData.name)
                                        updateConnections()
                                    }
                                    Rectangle {
                                        id: inPoint
                                        anchors.fill: parent
                                        color: inDropArea.containsDrag ? Qt.darker(modelData.color, 1.8) : modelData.color
                                        scale: inDropArea.containsDrag ? 1.1 : 1
                                        radius: height / 2
                                        Component.onCompleted: root.graphChanged.connect(saveCoords)
                                        Component.onDestruction: root.graphChanged.disconnect(saveCoords)
                                        function saveCoords() {
                                            if (modelData.isConnected) {
                                                var pos = mapToItem(canvas, width / 2, height / 2)
                                                root.portCoords[modelData.nodeName + modelData.name] = pos
                                            }
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            onEntered: {
                                                var pos = mapToItem(flick, width/2, height/2)
                                                draggedPoint.x = pos.x - draggedPoint.width/2
                                                draggedPoint.y = pos.y - draggedPoint.height/2
                                                draggedPoint.z = highestZ
                                                draggedPoint.nodeName = graphNode.name
                                                draggedPoint.portName = modelData.name
                                                draggedPoint.portType = "in_"
                                                draggedPoint.color = modelData.color
                                                draggedPoint.visible = true
                                            }
                                            onExited: {
                                                if (!draggedPoint.dragged)
                                                    draggedPoint.visible = false
                                            }
                                        }
                                    }
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
                        clip: true
                        spacing: 2
                        Repeater {
                            model: modelData.outputPorts
                            RowLayout {
                                width: outputPortColumn.width
                                Text { text: modelData.name; clip: true; elide: Text.ElideRight; horizontalAlignment: Qt.AlignRight; Layout.fillWidth: true }
                                DropArea {
                                    id: outDropArea
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: height
                                    Layout.alignment: Qt.AlignRight
                                    keys: [ "in_" + modelData.color ]
                                    onDropped: {
                                        graphCore.addGraphConnection(graphNode.name, modelData.name,
                                                                     draggedPoint.nodeName, draggedPoint.portName)
                                        updateConnections()
                                    }
                                    Rectangle {
                                        id: outPoint
                                        anchors.fill: parent
                                        color: outDropArea.containsDrag ? Qt.darker(modelData.color, 1.8) : modelData.color
                                        scale: outDropArea.containsDrag ? 1.1 : 1
                                        radius: height / 2
                                        Component.onCompleted: root.graphChanged.connect(saveCoords)
                                        Component.onDestruction: root.graphChanged.disconnect(saveCoords)
                                        function saveCoords() {
                                            if (modelData.isConnected) {
                                                var pos = mapToItem(canvas, width / 2, height / 2)
                                                root.portCoords[modelData.nodeName + modelData.name] = pos
                                            }
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            onEntered: {
                                                var pos = mapToItem(flick, width/2, height/2)
                                                draggedPoint.x = pos.x - draggedPoint.width/2
                                                draggedPoint.y = pos.y - draggedPoint.height/2
                                                draggedPoint.z = highestZ
                                                draggedPoint.nodeName = graphNode.name
                                                draggedPoint.portName = modelData.name
                                                draggedPoint.portType = "out_"
                                                draggedPoint.color = modelData.color
                                                draggedPoint.visible = true
                                            }
                                            onExited: {
                                                if (!draggedPoint.dragged)
                                                    draggedPoint.visible = false
                                            }
                                        }
                                    }
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
                        if (mouse.button === Qt.RightButton) {
                            nodeMenu.popup()
                        } else {
                            if (mouse.modifiers & Qt.ControlModifier) {
                                if (selectedNodes[graphNode.name] !== undefined)
                                    deselect(graphNode)
                                else
                                    select(graphNode, false)
                            } else {
                                select(graphNode, true)
                            }
                        }
                    }

                    Menu {
                        id: nodeMenu
                        MenuItem {
                            text: qsTr("Add Output Port...")
                            onTriggered: {
                                //                                var name = "NewPort"
                                //graphCore.addGraphConnection(name, mouseArea.mouseX, mouseArea.mouseY)
                            }
                        }
                        MenuItem { text: qsTr("Add Intput Port...") }
                    }
                }
            }
        }

        Rectangle {
            id: draggedPoint
            property alias dragged: mi.pressed
            property var sourcePos
            property string nodeName
            property string portName
            property string portType
            width: 16
            height: width
            radius: width / 2
            visible: false
            scale: zoomFactor
            onVisibleChanged: sourcePos = (visible ? Qt.point(x + width/2, y + height/2) : undefined)
            onXChanged: canvas.requestPaint()
            onYChanged: canvas.requestPaint()

            Drag.keys: [ portType + color ]
            Drag.active: mi.drag.active
            Drag.hotSpot.x: width / 2
            Drag.hotSpot.y: height / 2

            MouseArea {
                id: mi
                anchors.fill: parent
                drag.target: draggedPoint
                acceptedButtons: Qt.LeftButton
                scrollGestureEnabled: false
                onReleased: {
                    if (draggedPoint.Drag.target !== null)
                        draggedPoint.Drag.drop()
                    draggedPoint.visible = false
                    canvas.requestPaint()
                }
            }
        }

        Canvas {
            id: canvas
            anchors.fill: parent
            z: highestZ
            function paintCurve(ctx, start, end) {
                ctx.beginPath()
                ctx.moveTo(start.x, start.y)
                var center = Qt.point((start.x + end.x)/2, (start.y + end.y)/2)
                ctx.quadraticCurveTo(center.x, start.y, center.x, center.y)
                ctx.quadraticCurveTo(center.x, end.y, end.x, end.y)
                ctx.stroke()
            }
            onPaint: {
                var ctx = getContext("2d")
                ctx.save()
                ctx.clearRect(0, 0, canvas.width, canvas.height)
                ctx.lineWidth = 2
                for (var i = 0; i < graphCore.graphConnections.length; ++i) {
                    var conn = graphCore.graphConnections[i]
                    var id = conn.sourceNodeName + conn.outputPortName
                    var start = root.portCoords[id]
                    if (start === undefined)
                        continue;
                    id = conn.targetNodeName + conn.inputPortName
                    var end = root.portCoords[id]
                    if (end === undefined)
                        continue;
                    ctx.strokeStyle = conn.color
                    paintCurve(ctx, start, end)
                }
                if (draggedPoint.dragged) {
                    ctx.strokeStyle = draggedPoint.color
                    paintCurve(ctx, draggedPoint.sourcePos, Qt.point(draggedPoint.x + draggedPoint.width/2,
                                                                     draggedPoint.y + draggedPoint.height/2))
                }
                ctx.restore()
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
