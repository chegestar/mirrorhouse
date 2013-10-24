/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

import QtQuick 1.1

Item {
    id:ticketButton

    property int buttonId: 0

    signal btnClicked(int buttonId)

    function sendBtnClicked()
    {
        if (buttonId != 0) {
            btnClicked(buttonId);
        }
    }

    function resetImage()
    {
        imageId2.x = 0;
        imageId2.y = 0;
        imageId2.rotation = 0;
    }

    Image {
        id: imageId1
        fillMode: Image.PreserveAspectFit
        smooth: true

        source: {
            if (buttonId == 1) {
                "qrc:/gfx/exit_full.png";
            }
            else if (buttonId == 2) {
                "qrc:/gfx/info_full.png";
            }
            else if (buttonId == 3) {
                "qrc:/gfx/save_full.png";
            }
            else {
                console.log("Error: No buttonId defined!");
            }
        }
    }

    Image {
        id: imageId2
        fillMode: Image.PreserveAspectFit
        smooth: true

        source: {
            if (buttonId == 1) {
                "qrc:/gfx/exit_full.png";
            }
            else if (buttonId == 2) {
                "qrc:/gfx/info_full.png";
            }
            else if (buttonId == 3) {
                "qrc:/gfx/save_full.png";
            }
            else {
                console.log("Error: No buttonId defined!");
            }
        }
    }

    SequentialAnimation {
        id:btnAnim

        ParallelAnimation {
            NumberAnimation {
                target: imageId2
                property: "rotation"
                to: 20
                duration: 800
            }
            NumberAnimation {
                target: imageId2
                property: "x"
                easing.type:Easing.InBack
                to: imageId2.x + 100
                duration: 800
            }
            NumberAnimation {
                target: imageId2
                property: "y"
                easing.type:Easing.InBack
                to: imageId2.y + 120
                duration: 800
            }
        }
        ScriptAction { script: sendBtnClicked(); }
        ScriptAction { script: resetImage(); }
    }

    MouseArea {
        anchors.fill: imageId1
        onClicked: btnAnim.restart()
    }
}
