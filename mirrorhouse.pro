# Copyright (c) 2011-2012 Nokia Corporation.

TEMPLATE = app
TARGET = mirrorhouse
VERSION = 1.3.1

QT += network declarative opengl
CONFIG += mobility
MOBILITY = multimedia

INCLUDEPATH += src

HEADERS += \
    src/mirroreffect.h \
    src/mirroritem.h \
    src/myvideosurface.h \
    src/videoif.h
    
SOURCES += \
    src/main.cpp \
    src/mirroreffect.cpp \
    src/mirroritem.cpp \
    src/myvideosurface.cpp

OTHER_FILES += \
    qml/main.qml \
    qml/Mirror.qml \
    qml/FlipableMirror.qml \
    qml/TicketButton.qml \
    qml/Info.qml

symbian {
    TARGET = MirrorHouse
    TARGET.UID3 = 0xEF642F0F
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x20000 0x9000000

    # Self-signing capabilities
    TARGET.CAPABILITY += \
        ReadUserData \
        WriteUserData \
        UserEnvironment

    # For portrait orientation lock
    LIBS += -lcone -leikcore -lavkon

    RESOURCES += symbian_resources.qrc
    OTHER_FILES += qml/symbian.qml

    ICON = icons/symbian_icon.svg
}

contains(MEEGO_EDITION,harmattan) {
    DEFINES += Q_OS_HARMATTAN
    RESOURCES += harmattan_resources.qrc
    OTHER_FILES += qml/harmattan.qml

    target.path = /opt/$${TARGET}/bin

    desktopfile.files = $${TARGET}.desktop
    desktopfile.path = /usr/share/applications

    icon.files = icons/$${TARGET}.png
    icon.path = /usr/share/icons/hicolor/64x64/apps

    icon.files = icons/$${TARGET}.png
    icon.path = /usr/share/icons/hicolor/80x80/apps

    INSTALLS += target desktopfile icon
}

simulator {
    RESOURCES += symbian_resources.qrc
    OTHER_FILES += qml/symbian.qml
}
