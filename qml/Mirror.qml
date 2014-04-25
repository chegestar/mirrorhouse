/**
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

import QtQuick 1.1
import CustomItems 1.0

Item {
    id: mirror
    anchors.fill: parent
    property int mirrorEffectId

    // Create camera component
    function createCamera(enable) {
        if (enable && cameraLoader.item == undefined) {
            cameraLoader.sourceComponent = cameraComponent;
        }
    }

    // Delete camera component
    function deleteCamera() {
        cameraLoader.sourceComponent = undefined;
    }

    // Enable or disable camera
    function enableCamera(enable) {
        createCamera(enable);
        if (cameraLoader.item != undefined) {
            cameraLoader.item.enableEffect(mirrorEffectId,0,0);
            cameraLoader.item.enableCamera(enable);
        }
    }

    // Enable or disable camera by index
    function enableCameraAt(enable, index) {
        createCamera(enable);
        if (cameraLoader.item != undefined) {
            cameraLoader.item.enableEffect(mirrorEffectId,0,0);
            cameraLoader.item.enableCameraAt(enable,index);
        }
    }

    // Pause camera
    function pauseCamera() {
        if (cameraLoader.item != undefined)
            cameraLoader.item.pauseCamera();
    }

    // Save camera frame to Gallery
    function saveToFile() {
        if (cameraLoader.item != undefined)
            cameraLoader.item.saveToFile();
    }


    signal zoomIn()
    signal zoomOut()

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#FFFFFF" }
            GradientStop { position: 1.0; color: "#7F7F7F" }
        }
    }

    // Loader for dynamically creating MirrorItem
    Loader {
        id: cameraLoader
        anchors.fill: parent
        sourceComponent: undefined
    }


    // Custom camera component that shows mirror camera
    Component {
        id: cameraComponent
        MirrorItem {
            anchors.fill: parent
            effectId: mirror.mirrorEffectId

            onMinimizeMirror: {
                zoomOut();
            }
            onMaximizeMirror: {
                zoomIn();
            }
        }
    }

    Image {
        id: border
        smooth: true
        source: "qrc:/gfx/mirror.png"
        width: parent.width * 1.3
        height: parent.height * 1.5
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
}

