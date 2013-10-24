/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.1

Item {
    id: infoContainer
    opacity: 0

    property int parentWidth
    property int parentHeight

    function showInfo(width, height) {
        parentWidth = width;
        parentHeight = height;

        infoContainer.opacity = 0;

        infoContainer.width = parentWidth * 0.1;
        infoContainer.height = parentHeight * 0.2;
        infoContainer.x = parentWidth * 0.5;
        infoContainer.y = parentHeight * 0.5;

        infoAnim.restart();
    }

    Image {
        source: "qrc:/gfx/info_noshadow.png"
        smooth: true
        anchors.fill: parent
    }

    Image {
        id: closeImage
        source: "qrc:/gfx/info_close.png"
        smooth: true
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        anchors.horizontalCenter: parent.horizontalCenter
    }


    ParallelAnimation {
        id: infoAnim
        NumberAnimation { target: infoContainer; property: "opacity"; from:0; to: 1; duration: 1000 }
        PropertyAction { target: infoContainer; property: "opacity"; value: 1 }
        NumberAnimation { target: infoContainer; property: "width"; to: parentWidth; duration:1000 }
        NumberAnimation { target: infoContainer; property: "height"; to: parentHeight; duration: 1000;
            easing.type: Easing.InOutElastic; easing.amplitude: 2.0; easing.period: 1.5}
        NumberAnimation { target: infoContainer; property: "x"; to: 0; duration: 1000 }
        NumberAnimation { target: infoContainer; property: "y"; to: 0; duration: 1000 }
    }


    NumberAnimation {
        id: infoEndAnim
        target: infoContainer; property: "opacity"; from: 1; to: 0; duration: 1000
    }

    MouseArea {
        anchors.fill: parent
    }

    Flickable {
        id: flickable
        anchors {
            fill: parent
            leftMargin: 40
            rightMargin: 40
            topMargin: 150
            bottomMargin: 40
        }
        contentWidth: width
        contentHeight: infoText.height
        clip: true

        Text {
            id: infoText
            width: flickable.width
            color: "#602f05"
            font.pixelSize: flickable.width * 0.05

            text: "<a href=\"http://projects.developer.nokia.com/mirrorhouse\">"
                  + "<font color=\"#000000\">MirrorHouse v1.3</font></a><br><br>"
                  + "The MirrorHouse Qt Quick application demonstrates "
                  + "QCamera frame manipulation at runtime with "
                  + "different mirror effects.<br>"
                  + "Swipe the mirrors from right to left to get more mirrors visible. "
                  + "Tap the mirror for turning and pinch for zooming the mirror.<br><br>"
                  + "Copyright 2011-2012 Nokia Corporation. All rights reserved.";

            wrapMode: Text.WordWrap
            onLinkActivated: Qt.openUrlExternally(link);
        }
    }

    MouseArea {
        anchors.fill: closeImage
        onClicked: {
            infoEndAnim.restart();
        }
    }

}
