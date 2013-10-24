/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.1
import CustomItems 1.0

Flipable {
    id: flipableMirror
    objectName: "FlipableMirror"

    property bool flipped: false
    property bool showCamera: false
    property int mirrorEffectId

    property bool zoomedIn: false
    property bool rotationRunning: false
    property int defaultX
    property int defaultY
    property int defaultWidth
    property int defaultHeight


    // Mirror rotation hint anim
    function rotationAnimation() {
        rotationHintAnim.restart();
    }


    // Enable camera, 2. camera if not flipped and 1. camera if flipped
    function enableCamera(enable) {
        showCamera = enable;
        if (flipableMirror.flipped) {
            mirrorFront.enableCamera(false); // 1 camera
            mirrorBack.enableCameraAt(enable,0); // 0 camera
        } else {
            mirrorBack.enableCamera(false); // 0 camera
            mirrorFront.enableCameraAt(enable,1); // 1 camera
        }
    }

    function saveToFile() {
        if (flipableMirror.flipped) {
            mirrorBack.saveToFile();
        } else {
            mirrorFront.saveToFile();
        }
    }

    function pauseCamera() {
        showCamera = false;
        mirrorFront.pauseCamera();
        mirrorBack.pauseCamera();
    }

    function isZooming()
    {
        var ret = false;
        if (zoomInAnim.running || zoomOutAnim.running)
            ret = true;
        return ret;
    }

    function isRotating()
    {
        var ret = false;
        if (rotationRunning)
            ret = true;

        return ret;
    }

    function doZoomIn()
    {
        if (zoomedIn || isRotating()) {
            return;
        }
        if (!zoomInAnim.running && !zoomOutAnim.running) {
            main.disableAllCameras();
            flipableMirror.z = main.getZ();
            defaultWidth = flipableMirror.width;
            defaultHeight = flipableMirror.height;
            defaultX = flipableMirror.x;
            defaultY = flipableMirror.y;
            zoomInAnim.restart();
            enableCamera(true);
        }
    }

    function doZoomOut()
    {
        if (!zoomedIn || isRotating()) {
            return;
        }
        if (!zoomInAnim.running && !zoomOutAnim.running) {
            main.disableAllCameras();
            zoomOutAnim.restart();
            enableCamera(true);
        }
    }

    ParallelAnimation {
        id: zoomInAnim
        PropertyAction { target: flickable; property: "interactive"; value: false }
        NumberAnimation { target: flipableMirror; property: "width"; to: main.fixedWidth - 50; duration:1000 }
        NumberAnimation { target: flipableMirror; property: "height"; to: main.fixedHeight - 100; duration: 1000;
            easing.type: Easing.InOutElastic; easing.amplitude: 2.0; easing.period: 1.5}
        NumberAnimation { target: flipableMirror; property: "x"; to: flickable.contentX + 25; duration: 1000 }
        NumberAnimation { target: flipableMirror; property: "y"; to: flickable.yPos*-1 + 50; duration: 1000 }
        PropertyAction { target: flipableMirror; property: "zoomedIn"; value: true }
    }

    ParallelAnimation {
        id: zoomOutAnim
        NumberAnimation { target: flipableMirror; property: "width"; to: flipableMirror.defaultWidth; duration:1000 }
        NumberAnimation { target: flipableMirror; property: "height"; to: flipableMirror.defaultHeight; duration: 1000;
            easing.type: Easing.InOutElastic; easing.amplitude: 2.0; easing.period: 1.5}
        NumberAnimation { target: flipableMirror; property: "x"; to: flipableMirror.defaultX; duration: 1000 }
        NumberAnimation { target: flipableMirror; property: "y"; to: flipableMirror.defaultY; duration: 1000 }
        PropertyAction { target: flipableMirror; property: "zoomedIn"; value: false }
        PropertyAction { target: flickable; property: "interactive"; value: true }
    }

    front: Mirror {
        id: mirrorFront
        mirrorEffectId: flipableMirror.mirrorEffectId
        onZoomIn: {
            doZoomIn();
        }
        onZoomOut: {
            doZoomOut();
        }
    }
    back: Mirror {
        id: mirrorBack
        mirrorEffectId: flipableMirror.mirrorEffectId
        onZoomIn: {
            doZoomIn();
        }
        onZoomOut: {
            doZoomOut();
        }
    }

    Component.onCompleted: {
        if(showCamera) {
            enableCamera(true);
        }
    }

    // Rotate y-axcel
    transform: Rotation {
        id: rotation
        origin.x: flipableMirror.width/2
        origin.y: flipableMirror.height/2
        axis.x: 0; axis.y: 1; axis.z: 0
        angle: 0
    }

    states: State {
        name: "back"
        PropertyChanges { target: rotation; angle: 180 }
        when: flipableMirror.flipped
    }

    transitions: Transition {
        SequentialAnimation {
            PropertyAction { target: flipableMirror; property: "rotationRunning"; value: true }
            NumberAnimation {target: rotation; property: "angle"; duration: 3200; easing.type: Easing.Linear }
            PropertyAction { target: flipableMirror; property: "rotationRunning"; value: false }
        }
    }

    SequentialAnimation {
        id: rotationHintAnim
        PropertyAction { target: flipableMirror; property: "rotationRunning"; value: true }
        NumberAnimation {target: rotation; property: "angle"; from:0; to:120;
            duration: 2100; easing.type: Easing.Linear }
        PauseAnimation { duration: 300 }
        NumberAnimation {target: rotation; property: "angle"; from:120; to:0;
            duration: 2100; easing.type: Easing.Linear }
        PropertyAction { target: flipableMirror; property: "rotationRunning"; value: false }
    }


}
