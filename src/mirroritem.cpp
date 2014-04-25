/**
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

#include "mirroritem.h"

#include <QCameraImageCapture>
#include <QCameraViewfinder>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTimer>
#include <QTouchEvent>
#include <QVideoRendererControl>

#include "mirroreffect.h"
#include "myvideosurface.h"


/*!
  \class MirrorItem
  \brief The actual mirror that shows mirror customized camera viewfinder pictures
*/


/*!
  Constructor.
*/
MirrorItem::MirrorItem(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    m_camera(0),
    m_myVideoSurface(0),
    m_strength(0.0f),
    m_count(0.0f),
    m_deviceId(0),
    m_effectId(MirrorEffect::None),
    m_pinchCounter(0),
    m_showViewFinder(false),
    m_keepPaintingStoredPicture(false),
    m_signalSent(false)
{
    // Important, otherwise the paint method is never called
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    setFlag(ItemIsMovable);

    // Enable multitouch/listen to touch events
    setAcceptTouchEvents(true);

    // Get the list of available camera devices
    m_devices = QCamera::availableDevices();
}


/*!
  Destructor.
*/
MirrorItem::~MirrorItem()
{
    if (m_myVideoSurface) {
        m_myVideoSurface->stop();
    }

    if (m_camera) {
        m_camera->stop();
    }

    delete m_camera;
}


/*!
  From QDeclarativeItem.

  Handles the touch events for minimizing and maximizing the mirror.
*/
bool MirrorItem::sceneEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd: {
        if (event->type() == QEvent::TouchBegin) {
            m_pinchCounter = 3;
            m_signalSent = false;
        }
        else if (event->type() == QEvent::TouchEnd) {
            m_signalSent = false;
        }

        QTouchEvent *touchEvent = static_cast<QTouchEvent*>(event);
        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();

        if (touchPoints.count() == 2) {
            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
            const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
            qreal posLineLength = QLineF(touchPoint0.pos(), touchPoint1.pos()).length();
            qreal startPosLineLength = QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
            qreal scaleVal = posLineLength / startPosLineLength;

            if (scaleVal < 1.0) {
                m_pinchCounter--;
                if (m_pinchCounter < 0) {
                    if (!m_signalSent) {
                        emit minimizeMirror();
                    }
                    m_signalSent = true;
                }
            }
            else {
                m_pinchCounter++;
                if (m_pinchCounter > 6) {
                    if (!m_signalSent) {
                        emit maximizeMirror();
                    }
                    m_signalSent = true;
                }
            }
        }

        return true;
    }
    default:
        break;
    } // switch (event->type())

    return QDeclarativeItem::event(event);
}


/*!
  From QDeclarativeItem.
  Paits the customized mirror
*/
void MirrorItem::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing, false);

    if (m_myVideoSurface && m_myVideoSurface->framesExists()) {
        // Show the view finder
        m_myVideoSurface->paint(painter);
    }
    else if (m_lastPicture.byteCount() > 0) {
        // Keep painting last stored QImage picture
        if (boundingRect().width() != m_lastPicture.width()) {
            // Need to scale QImage picture
            painter->drawImage(boundingRect(),m_lastPicture);
        } else {
            // No scale needed
            painter->drawImage(0,0,m_lastPicture);
        }
    }
    else {
        // Draw an empty mirror
        painter->fillRect(boundingRect(), Qt::black);
    }
}


/*!
  If the view finder is shown, paints this item.
*/
void MirrorItem::updateVideo()
{
    if (m_showViewFinder) {
        update();
    }
}

/*!
  Current used effect id
*/
int MirrorItem::effectId() const
{
    return m_effectId;
}


/*!
  Set current effect id
*/
void MirrorItem::setEffectId(int effect)
{
    if (m_effectId != effect) {
        m_effectId = effect;
        emit effectIdChanged(m_effectId);
    }
}


/*!
*/
void MirrorItem::enableCamera(QVariant enable)
{
    bool b = enable.toBool();

    if (b) {
        // Enable the camera device
        QTimer::singleShot(3000, this, SLOT(startCamera()));
    }
    else {
        // Keep painting the last image read from the camera
        keepPaintingStoredPicture();

        // Stop the camera device
        stopCamera();
    }
}


/*!
  Start camera using device index
*/
void MirrorItem::enableCameraAt(QVariant enable, QVariant cameraIndex)
{
    if (enable.toBool()) {
        stopCamera();
    }

    m_deviceId = cameraIndex.toInt();

#ifdef Q_OS_HARMATTAN
    m_deviceId--;

    if (m_deviceId < 0)
        m_deviceId = 1;
#endif

    enableCamera(enable);
}


/*!
  Pauses the camera and keeps painting the store picture.
*/
void MirrorItem::pauseCamera()
{
    keepPaintingStoredPicture();
    enableCamera(false);
}


/*!
  Enable mirror effect
*/
void MirrorItem::enableEffect(QVariant id, QVariant strength, QVariant count)
{
    setEffectId(id.toInt());
    m_strength = strength.toDouble();
    m_count = count.toDouble();
}


/*!
  Save mirror to file (Gallery)
*/
void MirrorItem::saveToFile()
{
    if (m_myVideoSurface && m_myVideoSurface->framesExists()) {
        QImage image(m_myVideoSurface->targetImage().size(),
                     m_myVideoSurface->targetImageFormat());
        QPainter painter(&image);
        m_myVideoSurface->paint(&painter);
        doSave(image);
    }
    else if (m_lastPicture.width() > 0) {
        doSave(m_lastPicture);
    }
}


/*!
 Start camera device
*/
void MirrorItem::startCamera()
{
    if (m_camera || m_myVideoSurface) {
        stopCamera();
        enableCamera(QVariant(true));
        return;
    }

    if (m_deviceId > m_devices.count() - 1) {
        m_deviceId = 0;
    }

    m_camera = new QCamera(m_devices[m_deviceId]);
    connect(m_camera, SIGNAL(error(QCamera::Error)),
            this, SLOT(handleCameraError(QCamera::Error)));

    // Own video output for drawing
    QMediaService *mediaService = m_camera->service();
    QVideoRendererControl *rendererControl =
            mediaService->requestControl<QVideoRendererControl*>();
    m_myVideoSurface = new MyVideoSurface(this, this, this);
    m_myVideoSurface->enableEffect(m_effectId, m_strength, m_count);

    // The video renderer control takes ownership of the surface
    rendererControl->setSurface(m_myVideoSurface);

    // Start the camera
    m_camera->start();
    m_showViewFinder = true;
}


/*!
  Stop camera device
*/
void MirrorItem::stopCamera()
{
    // Stop the camera device
    m_showViewFinder = false;

    if (m_myVideoSurface) {
        m_myVideoSurface->stop();
        m_myVideoSurface->releaseMemory();
        m_myVideoSurface = 0;
    }

    if (m_camera) {
        m_camera->stop();
    }

    delete m_camera;
    m_camera = 0;
    m_deviceId = 0;
}


/*!
  Camera send error
*/
void MirrorItem::handleCameraError(QCamera::Error error)
{
    qDebug() << "MirrorItem::handleCameraError(): QCamera::Error:"
             << error << ";"
             << m_camera->errorString();
}


/*!
  Paint last camera frame
*/
void MirrorItem::keepPaintingStoredPicture()
{
    if (m_myVideoSurface && m_myVideoSurface->framesExists()) {
        m_lastPicture = m_myVideoSurface->targetImage();
    }
}


/*!
  Saves \a image into a file with name of type "mirror_<number>.jpg".
*/
void MirrorItem::doSave(QImage image)
{
#ifdef Q_OS_HARMATTAN
    QString path(QDesktopServices::storageLocation(QDesktopServices::HomeLocation)
                 + QString("/MyDocs/Pictures"));
#else
    QString path(QDesktopServices::storageLocation(QDesktopServices::PicturesLocation));
#endif

    QDir dir(path);

    // Get next filename
    QStringList files = dir.entryList(QStringList() << "mirror_*.jpg");
    int lastImage = 0;

    foreach(QString fileName, files) {
        int imgNumber = fileName.mid(7, fileName.size() - 11).toInt();
        lastImage = qMax(lastImage, imgNumber);
    }

    QString newFileName = QString("mirror_%1.jpg").arg(lastImage + 1);
    path.append("/");
    path.append(newFileName);

    image.save(path);
}
