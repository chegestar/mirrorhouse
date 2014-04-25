/**
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

import QtQuick 1.1
import CustomItems 1.0

AppWindow {
    id: main
    anchors.fill: parent

    property int mirrorWidth: fixedWidth * 0.7
    property int mirrorHeight: fixedHeight * 0.6
    property int zCounter:0
    property bool applicationTouched: false
    property int idleAnimCount: 0

    // Width and height are the other way around on Harmattan compared to Symbian
    property int fixedWidth: width < height ? width : height;
    property int fixedHeight: width < height ? height : width;


    // Background
    Image {
        id: backround
        source: "qrc:/gfx/mirrorhouse_background.png"
        anchors.fill: parent
    }

    function getZ() {
        zCounter++;
        return 100 + zCounter;
    }

    // Show InfoView
    function showInfo()
    {
        // Create InfoView dynamically
        if (infoLoader.status == Loader.Null)
            infoLoader.source = "qrc:/Info.qml";
        infoLoader.item.showInfo(fixedWidth, fixedHeight);

    }

    function disableAllCameras() {
        mirror1.enableCamera(false);
        mirror2.enableCamera(false);
        mirror3.enableCamera(false);
        mirror4.enableCamera(false);
        mirror5.enableCamera(false);
        mirror6.enableCamera(false);
    }

    function pauseAllCameras() {
        mirror1.pauseCamera();
        mirror2.pauseCamera();
        mirror3.pauseCamera();
        mirror4.pauseCamera();
        mirror5.pauseCamera();
        mirror6.pauseCamera();
    }

    function enableMirrorId(id) {
        main.disableAllCameras();
        if (mirror1 == id)
            mirror1.enableCamera(true);
        else if (mirror2 == id)
            mirror2.enableCamera(true);
        else if (mirror3 == id)
            mirror3.enableCamera(true);
        else if (mirror4 == id)
            mirror4.enableCamera(true);
        else if (mirror5 == id)
            mirror5.enableCamera(true);
        else if (mirror6 == id)
            mirror6.enableCamera(true);

    }

    // Buttons
    TicketButton {
        id: saveBtn
        buttonId: 3
        x: fixedWidth * 0.05
        y: fixedHeight * 0.88
    }
    Connections {
        target: saveBtn
        onBtnClicked: {
            var currentItem = flickable.contentItem.childAt(flickable.contentX + mirrorWidth / 2,
                                                            flickable.contentY + mirrorHeight / 2);
            if (currentItem != null) {
                currentItem.saveToFile();
                wait.showFileWait();
            }
        }
    }

    // Info button
    TicketButton {
        id: infoButton
        buttonId: 2
        x: isHarmattan ? fixedWidth * 0.75 : fixedWidth * 0.4;
        y: fixedHeight * 0.88
    }
    Connections {
        target: infoButton
        onBtnClicked: main.showInfo();
    }

    // Exit button. Not visible in Harmattan build.
    TicketButton {
        id: exitButton
        buttonId: 1
        x: fixedWidth * 0.75
        y: fixedHeight * 0.88
        visible: !isHarmattan
    }
    Connections {
        target: exitButton
        onBtnClicked: Qt.quit();
    }

    // Scrollbar showing current mirror
    Rectangle {
        id: scrollbar
        anchors.top: parent.top
        x: flickable.visibleArea.xPosition * flickable.width
        height: 5
        width: flickable.visibleArea.widthRatio * flickable.width
        color: "black"
    }

    // Mirrors
    Flickable {
        id: flickable
        flickableDirection: Flickable.HorizontalFlick
        width: fixedWidth
        height: mirrorHeight
        y: yPos
        contentWidth: 6*mirrorWidth + 7*cap
        contentHeight: mirrorHeight

        property int cap: (fixedWidth - mirrorWidth) / 2
        property int yPos: (fixedHeight - height) * 0.5

        onFlickStarted: {
            pauseAllCameras();
        }

        FlipableMirror {
            id: mirror1
            x: flickable.cap
            mirrorEffectId: 1
            showCamera: true
            width: mirrorWidth
            height: mirrorHeight
            MouseArea {
                anchors.fill: parent
                anchors.bottomMargin: -20
                onClicked: {
                    if (mirror1.isZooming())
                        return;
                    if (mirror1.showCamera) {
                        mirror1.flipped = !mirror1.flipped
                        enableMirrorId(mirror1);
                    } else {
                        enableMirrorId(mirror1);
                        wait.showCameraWait();
                    }
                }
            }
        }
        FlipableMirror {
            id: mirror2
            x: mirrorWidth + flickable.cap * 2
            mirrorEffectId: 2
            showCamera: false
            width: mirrorWidth
            height: mirrorHeight
            MouseArea {
                anchors.fill: parent
                anchors.bottomMargin: -20
                onClicked: {
                    if (mirror2.isZooming())
                        return;
                    if (mirror2.showCamera) {
                        mirror2.flipped = !mirror2.flipped
                        enableMirrorId(mirror2);
                    } else {
                        enableMirrorId(mirror2);
                        wait.showCameraWait();
                    }
                }
            }
        }
        FlipableMirror {
            id: mirror3
            x: mirrorWidth * 2 + flickable.cap * 3
            mirrorEffectId: 3
            showCamera: false
            width: mirrorWidth
            height: mirrorHeight
            MouseArea {
                anchors.fill: parent
                anchors.bottomMargin: -20
                onClicked: {
                    if (mirror3.isZooming())
                        return;
                    if (mirror3.showCamera) {
                        mirror3.flipped = !mirror3.flipped
                        enableMirrorId(mirror3);
                    } else {
                        enableMirrorId(mirror3);
                        wait.showCameraWait();
                    }
                }
            }
        }

        FlipableMirror {
            id: mirror4
            x: mirrorWidth * 3 + flickable.cap * 4
            mirrorEffectId: 4
            showCamera: false
            width: mirrorWidth
            height: mirrorHeight
            MouseArea {
                anchors.fill: parent
                anchors.bottomMargin: -20
                onClicked: {
                    if (mirror4.isZooming())
                        return;
                    if (mirror4.showCamera) {
                        mirror4.flipped = !mirror4.flipped
                        enableMirrorId(mirror4);
                    } else {
                        enableMirrorId(mirror4);
                        wait.showCameraWait();
                    }
                }
            }
        }
        FlipableMirror {
            id: mirror5
            x: mirrorWidth * 4 + flickable.cap * 5
            mirrorEffectId: 7
            showCamera: false
            width: mirrorWidth
            height: mirrorHeight
            MouseArea {
                anchors.fill: parent
                anchors.bottomMargin: -20
                onClicked: {
                    if (mirror5.isZooming())
                        return;
                    if (mirror5.showCamera) {
                        mirror5.flipped = !mirror5.flipped
                        enableMirrorId(mirror5);
                    } else {
                        enableMirrorId(mirror5);
                        wait.showCameraWait();
                    }
                }
            }
        }
        FlipableMirror {
            id: mirror6
            x: mirrorWidth * 5 + flickable.cap * 6
            mirrorEffectId: 8
            showCamera: false
            width: mirrorWidth
            height: mirrorHeight
            MouseArea {
                anchors.fill: parent
                anchors.bottomMargin: -20
                onClicked: {
                    if (mirror6.isZooming())
                        return;
                    if (mirror6.showCamera) {
                        mirror6.flipped = !mirror6.flipped
                        enableMirrorId(mirror6);
                    } else {
                        enableMirrorId(mirror6);
                        wait.showCameraWait();
                    }
                }
            }
        }


    }

    Component.onCompleted: {
        wait.showCameraWait();
        applicationTouchedTimer.restart();
    }


    // Wait note (opening camera)
    Item {
        id: wait
        opacity: 0
        smooth: true
        property bool ready: false

        function setPosition(mirror) {
            wait.width = mirror.width;
            wait.height = mirror.height / 5;
            wait.x = mirror.x - flickable.contentX
            wait.y = fixedHeight * 0.4;
        }

        function showFileWait() {
            textId.text = "Image of the mirror saved into Gallery";
            wait.width = mirrorWidth;
            wait.height = mirrorHeight / 5;
            wait.x = (fixedWidth - wait.width) / 2
            wait.y = fixedHeight * 0.4;
            popAnim.restart();
        }

        function showCameraWait() {
            textId.text = "Preparing effect\nand opening camera";
            show();
        }

        function show() {
            ready = false;
            if (mirror1.showCamera) {
                setPosition(mirror1);
                ready = true;
            } else if(mirror2.showCamera) {
                setPosition(mirror2);
                ready = true;
            } else if(mirror3.showCamera) {
                setPosition(mirror3);
                ready = true;
            } else if(mirror4.showCamera) {
                setPosition(mirror4);
                ready = true;
            }  else if(mirror5.showCamera) {
                setPosition(mirror5);
                ready = true;
            }  else if(mirror6.showCamera) {
                setPosition(mirror6);
                ready = true;
            }
            if (ready)
                popAnim.restart();

        }

        Image {
            anchors.fill: parent
            smooth: true
            source:"qrc:/gfx/info_small.png"
        }

        Text {
            id: textId
            anchors.fill: parent
            color: "#602f05"
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: parent.height * 0.25
        }
        SequentialAnimation {
            id: popAnim
            PropertyAction { target: flickable; property: "interactive"; value: false }
            PropertyAction { target: wait; property: "opacity"; value: 1 }
            PauseAnimation { duration: 3000 }
            PropertyAction { target: wait; property: "opacity"; value: 0 }
            PropertyAction { target: flickable; property: "interactive"; value: true }
        }
    }

    // Is application touched?
    MouseArea {
        anchors.fill: parent
        onPressed:  {
            mouse.accepted = false;
            applicationTouchedTimer.stop();
            moveMirrorsAnim.stop();
            main.applicationTouched = true;
        }
    }

    // Timer for application idle timing
    Timer {
        id: applicationTouchedTimer
        interval: 7000
        repeat: true
        onTriggered: {
            idleAnimCount++;
            if (idleAnimCount > 2) {
                applicationTouchedTimer.stop();
            }

            if (main.applicationTouched == false) {
                var mirrorItem = flickable.contentItem.childAt(flickable.contentX + mirrorWidth / 2,
                                                               flickable.contentY + mirrorHeight / 2);
                // First anim mirror rotation
                if (mirrorItem != null && idleAnimCount < 2) {
                    applicationTouchedTimer.interval = 5000
                    mirrorItem.rotationAnimation();
                }
                // Second anim mirror moving
                else if (idleAnimCount < 3) {
                    moveMirrorsAnim.restart();
                }
            }
        }
    }


    Loader {
        id: infoLoader
    }

    // Animation that shows mirror moving animiation while user idle
    SequentialAnimation {
        id: moveMirrorsAnim
        NumberAnimation {target: flickable; property: "contentX";
            to:flickable.contentX + mirrorWidth * 0.8;
            duration: 2000; easing.type: Easing.Linear }
        NumberAnimation {target: flickable; property: "contentX";
            to:0;
            duration: 2000; easing.type: Easing.Linear }
    }

}
