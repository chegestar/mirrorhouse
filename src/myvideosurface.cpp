/**
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

#include "myvideosurface.h"

#include <QDebug>
#include <QDeclarativeItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "videoif.h"


/*!
  \class MyVideoSurface
  \brief Class for reading camera viewfinder frames for manipulating them
*/


/*!
  Constructor.
*/
MyVideoSurface::MyVideoSurface(QDeclarativeItem *targetItem,
                               VideoIF *target,
                               QObject *parent)
    : QAbstractVideoSurface(parent),
      m_targetItem(targetItem),
      m_target(target),
      m_mirrorEffect(0),
      m_imageFormat(QImage::Format_Invalid),
      m_strength(0.0f),
      m_count(0.0f),
      m_effectId(MirrorEffect::None),
      m_framesExists(false)
{
    setError(QAbstractVideoSurface::NoError);
}


/*!
  Destructor.
*/
MyVideoSurface::~MyVideoSurface()
{
    if (m_mirrorEffect)
        delete m_mirrorEffect;
}

/*!
  Free memory
*/
void MyVideoSurface::releaseMemory()
{
    delete m_mirrorEffect;
    m_mirrorEffect = 0;
}

/*!
  From QAbstractVideoSurface.
*/
QList<QVideoFrame::PixelFormat> MyVideoSurface::supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_UYVY;
    }

    return QList<QVideoFrame::PixelFormat>();
}


/*!
  From QAbstractVideoSurface.
*/
QVideoSurfaceFormat MyVideoSurface::nearestFormat(
        const QVideoSurfaceFormat &format) const
{
    return format;
}


/*!
  From QAbstractVideoSurface.
*/
bool MyVideoSurface::start(const QVideoSurfaceFormat &format)
{
    m_framesExists = false;
    m_videoFormat = format;
    QImage::Format imageFormat =
            QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());

    // Force RGB32 - conversion will be done later.
    if (imageFormat == QImage::Format_Invalid)
        imageFormat = QImage::Format_RGB32;

    const QSize size = format.frameSize();

    if (imageFormat != QImage::Format_Invalid && !size.isEmpty()) {
        m_imageFormat = imageFormat;
        QAbstractVideoSurface::start(format);
        return true;
    }

    return false;
}


/*!
  From QAbstractVideoSurface.
*/
bool MyVideoSurface::present(const QVideoFrame &frame)
{
    // Handle frame
    m_frame = frame;

    if (surfaceFormat().pixelFormat() != m_frame.pixelFormat()
            || surfaceFormat().frameSize() != m_frame.size())
    {
        stop();
        return false;
    }

    if (m_frame.map(QAbstractVideoBuffer::ReadOnly)) {

        if(!m_mirrorEffect)
            m_mirrorEffect = new MirrorEffect();


        m_mirrorEffect->setSource((unsigned int*)m_frame.bits(),
                                  m_frame.width(),
                                  m_frame.height(),
                                  m_frame.bytesPerLine() / 4);

        // RGB or UYVY
        if (frame.pixelFormat() == QVideoFrame::Format_UYVY) {
            convertFrameData(frame);

            bool flipY(false);

            if (m_frame.width() < 600) {
                // Harmattan's frontcamera is in use. It must be flipped in
                // order to be correctly rotated
                flipY = true;
            }

            m_mirrorEffect->setSource(m_convertedImage.m_data,
                                      m_convertedImage.m_width,
                                      m_convertedImage.m_height,
                                      m_convertedImage.m_width,
                                      true,
                                      flipY);
        }
        else if (frame.pixelFormat() == QVideoFrame::Format_RGB32) {
            m_mirrorEffect->setSource((unsigned int*)m_frame.bits(),
                                      m_frame.width(),
                                      m_frame.height(),
                                      m_frame.bytesPerLine() / 4);
        }

        // Target
        if (m_targetImage.width() == 0) {
            // Make QImage from frame
            // Using smaller target picture that source
            m_targetImage = QImage(m_targetItem->boundingRect().width(),
                                   m_targetItem->boundingRect().height(),
                                   m_imageFormat);

            // Set target
            m_mirrorEffect->setTarget((unsigned int*)m_targetImage.bits(),
                                      m_targetImage.width(),
                                      m_targetImage.height(),
                                      m_targetImage.bytesPerLine() / 4);
        }

        // Set effect
        MyVideoSurface::setMirrorTransform(m_mirrorEffect, m_effectId);


        // Effect quality selection
        if (m_targetImage.size().width() > 300) {
            m_mirrorEffect->setHighQuality(false);
        }
        else {
            m_mirrorEffect->setHighQuality(true);
        }

        // Make effect
        m_mirrorEffect->process();

        m_frame.unmap();

        m_framesExists = true;

        // Update widget
        m_target->updateVideo();
    }

    return true;
}


/*!
  Camera frames coming
*/
bool MyVideoSurface::framesExists() const
{
    return m_framesExists;
}


/*!
  Target where the mirror is drawed
*/
QImage MyVideoSurface::targetImage() const
{
    return m_targetImage;
}


/*!
  Image format
*/
QImage::Format MyVideoSurface::targetImageFormat() const
{
    return m_imageFormat;
}


/*!
  Enable effect
*/
void MyVideoSurface::enableEffect(int id, double strength, double count)
{
    m_effectId = id;
    m_strength = strength;
    m_count = count;
}


/*!
  Paints the mirrored image.
*/
void MyVideoSurface::paint(QPainter *painter)
{
    if (m_targetImage.byteCount() > 0) {
        painter->drawImage(0, 0, m_targetImage);
    }
}


/*!
  From QAbstractVideoSurface.
*/
bool MyVideoSurface::isFormatSupported(const QVideoSurfaceFormat &format,
                                       QVideoSurfaceFormat *similar) const
{
    Q_UNUSED(similar);

    const QImage::Format imageFormat =
            QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    return (imageFormat != QImage::Format_Invalid
            && !size.isEmpty()
            && format.handleType() == QAbstractVideoBuffer::NoHandle);
}


/*!
  Sets the mirror house effect according to \a effect.
*/
void MyVideoSurface::setMirrorTransform(MirrorEffect *mirrorEffect,
                                        int effect)
{
    switch (effect) {
    case 1: {
        mirrorEffect->setMirrorTransform(MirrorEffect::Bubbles, 0.5f, 1.0f);
        break;
    }
    case 2: {
        mirrorEffect->setMirrorTransform(MirrorEffect::InvBubbles, 0.5f, 1.0f);
        break;
    }
    case 3: {
        mirrorEffect->setMirrorTransform(MirrorEffect::VerticalWave, 0.5f, 6.0f);
        break;
    }
    case 4: {
        mirrorEffect->setMirrorTransform(MirrorEffect::HorizontalWave, 0.5f, 6.0f);
        break;
    }
    case 5: {
        mirrorEffect->setMirrorTransform(MirrorEffect::Spiral, 0.5f, 1.0f);
        break;
    }
    case 6: {
        mirrorEffect->setMirrorTransform(MirrorEffect::Spiral, 0.7f, 1.0f);
        break;
    }
    case 7: {
        mirrorEffect->setMirrorTransform(MirrorEffect::Ripple, 0.6f, 4.0f);
        break;
    }
    case 8: {
        mirrorEffect->setMirrorTransform(MirrorEffect::Spike, 1.0f, 1.0f);
        break;
    }
    case 9: {
        mirrorEffect->setMirrorTransform(MirrorEffect::Tile, 1.0f, 14.0f);
        break;
    }
    case 10: {
        mirrorEffect->setMirrorTransform(MirrorEffect::Dither, 0.04f, 1.0f);
        break;
    }
    default: {
        mirrorEffect->setMirrorTransform(MirrorEffect::None);
        break;
    }
    } // switch (effect)
}


/*!
  Converts the frame data without the 90 degrees rotation.
*/
void MyVideoSurface::convertFrameData(const QVideoFrame &source)
{
    // From UYVY to RGB32
    if (!m_convertedImage.m_data
            || source.width() != m_convertedImage.m_width
            || source.height() != m_convertedImage.m_height)
    {
        if (m_convertedImage.m_data)
            delete[] m_convertedImage.m_data;

        m_convertedImage.m_width = source.width();
        m_convertedImage.m_height = source.height();
        m_convertedImage.m_data =
                new unsigned int[m_convertedImage.m_width
                                 * m_convertedImage.m_height];
    }

    signed int y1;
    signed int y2;
    signed int rfac;
    signed int gfac;
    signed int bfac;
    signed int r;
    signed int g;
    signed int b;

    for (int y = 0; y < m_convertedImage.m_height; ++y) {
        unsigned int *t = m_convertedImage.m_data + y * m_convertedImage.m_width;
        unsigned int *t_target = t + m_convertedImage.m_width;
        const unsigned int *s =
                (const unsigned int*)((const uchar*)source.bits()
                                      + source.bytesPerLine() * y);

        while (t != t_target) {
            // use y1 and y2 as u/v temps before they are overridden by actual y's
            y1 = (int)((*s >> 0) & 255) - 128;
            y2 = (int)((*s >> 16) & 255) - 128;

            bfac = (int)((y1 * 517) >> 8);
            gfac = (int)((y2 * 208) >> 8) - (int)((y1 * 100) >> 8);
            rfac = (int)((y2 * 409) >> 8);

            y1 = ((((int)((*s >> 8) & 255) - 16) * 298) >> 8);
            y2 = ((((int)((*s >> 24) & 255) - 16) * 298) >> 8);

            r = y1 + rfac;
            g = y1 - gfac;
            b = y1 + bfac;

            if (r < 0) r = 0;
            if (g < 0) g = 0;
            if (b < 0) b = 0;
            if (r > 255) r = 255;
            if (g > 255) g = 255;
            if (b > 255) b = 255;

            *t = b | (g << 8) | (r << 16) | 0xFF000000;

            t++;

            r = y2 + rfac;
            g = y2 - gfac;
            b = y2 + bfac;

            if (r < 0) r = 0;
            if (g < 0) g = 0;
            if (b < 0) b = 0;
            if (r > 255) r = 255;
            if (g > 255) g = 255;
            if (b > 255) b = 255;

            *t = b | (g << 8) | (r << 16) | 0xFF000000;

            t++;
            s++;
        }
    }
}
