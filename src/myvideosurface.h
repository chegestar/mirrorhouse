/**
 * Copyright (c) 2011-2012 Nokia Corporation.
 */

#ifndef MYVIDEOSURFACE_H
#define MYVIDEOSURFACE_H

#include <QAbstractVideoSurface>
#include <QImage>
#include <QList>
#include <QVideoSurfaceFormat>

#include "mirroreffect.h"

// Forward declarations
class MirrorEffect;
class QDeclarativeItem;
class QPainter;
class VideoIF;


/*!
  \class MyVideoSurface
  \brief Class for reading camera viewfinder frames for manipulating them
*/
class MyVideoSurface: public QAbstractVideoSurface
{
    Q_OBJECT

public:
    explicit MyVideoSurface(QDeclarativeItem *targetItem,
                            VideoIF *target,
                            QObject *parent = 0);
    ~MyVideoSurface();

public: // From QAbstractVideoSurface
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType =
                QAbstractVideoBuffer::NoHandle) const;
    QVideoSurfaceFormat	nearestFormat(const QVideoSurfaceFormat &format) const;
    bool start(const QVideoSurfaceFormat &format);
    bool present(const QVideoFrame &frame);

public:
    bool framesExists() const;
    QImage targetImage() const;
    QImage::Format targetImageFormat() const;
    void enableEffect(int id, double strength, double count);
    void paint(QPainter *painter);
    bool isFormatSupported(const QVideoSurfaceFormat &format,
                           QVideoSurfaceFormat *similar) const;
    static void setMirrorTransform(MirrorEffect *mirrorEffect, int effect);

    void releaseMemory();

private:
    void convertFrameData(const QVideoFrame &source);

private: // Data
    QDeclarativeItem *m_targetItem;
    VideoIF *m_target;
    MirrorEffect *m_mirrorEffect;
    QVideoFrame m_frame;
    QImage::Format m_imageFormat;
    QVideoSurfaceFormat m_videoFormat;
    QImage m_sourceImage;
    QImage m_targetImage;
    MirrorEffect::ImageProperties m_convertedImage;
    double m_strength;
    double m_count;
    int m_effectId;
    bool m_framesExists;
};

#endif // MYVIDEOSURFACE_H
