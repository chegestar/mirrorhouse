/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#ifndef MIRRORITEM_H
#define MIRRORITEM_H

#include <QByteArray>

// Unlike the other APIs in Qt Mobility, the Qt Mobility Multimedia API is not
// in the Qt Mobility namespace.
#include <QCamera>

#include <QDeclarativeItem>
#include <QImage>
#include <QList>
#include <QVariant>

#include "videoif.h"

// Forward declarations
class MyVideoSurface;
class QEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;


/*!
  \class MirrorItem
  \brief The actual mirror that shows mirror customized camera viewfinder pictures
*/
class MirrorItem : public QDeclarativeItem, public VideoIF
{
    Q_OBJECT
    Q_PROPERTY(int effectId READ effectId WRITE setEffectId NOTIFY effectIdChanged)

public:
    explicit MirrorItem(QDeclarativeItem *parent = 0);
    virtual ~MirrorItem();

public: // From QDeclarativeItem
    bool sceneEvent(QEvent *event);
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

public:
    void updateVideo();

    // Property setters and getters
    int effectId() const;
    void setEffectId(int id);

public slots:
    void enableCamera(QVariant enable);
    void enableCameraAt(QVariant enable, QVariant cameraIndex);
    void pauseCamera();
    void enableEffect(QVariant id, QVariant strength, QVariant count);
    void saveToFile();

private slots:
    void startCamera();
    void stopCamera();
    void handleCameraError(QCamera::Error);

private:
    void keepPaintingStoredPicture();
    void doSave(QImage image);

signals:
    void minimizeMirror();
    void maximizeMirror();

    // Property signals
    void effectIdChanged(int id);

private: // Data
    QCamera* m_camera; // Owned
    MyVideoSurface* m_myVideoSurface; // Not owned
    QImage m_lastPicture;
    QList<QByteArray> m_devices;
    double m_strength;
    double m_count;
    int m_deviceId;
    int m_effectId;
    int m_pinchCounter;
    bool m_showViewFinder;
    bool m_keepPaintingStoredPicture;
    bool m_signalSent;
};

QML_DECLARE_TYPE(MirrorItem)

#endif // MIRRORITEM_H
