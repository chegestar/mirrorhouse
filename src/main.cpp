/**
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

#include <QApplication>
#include <QDeclarativeContext>
#include <QDeclarativeView>
#include <QDebug>

// Lock Symbian orientation
#ifdef Q_OS_SYMBIAN
    #include <aknappui.h>
    #include <aknenv.h>
    #include <eikappui.h>
    #include <eikenv.h>
#endif

#ifdef Q_OS_SYMBIAN
    #include <QSymbianEvent>
    #include <w32std.h>
#endif

#include "mirroritem.h"

static const int KGoomMemoryLowEvent = 0x10282DBF;
static const int KGoomMemoryGoodEvent = 0x20026790;


class MyApplication : public QApplication
{
public:
    MyApplication(int argc, char** argv)
        : QApplication( argc, argv ) {}

#ifdef Q_OS_SYMBIAN
protected:
    bool symbianEventFilter(const QSymbianEvent* symbianEvent)
    {
        const TWsEvent *event = symbianEvent->windowServerEvent();

        if (!event) {
            return false;
        }

        switch (event->Type()) {
            // GOOM handling enabled
            // http://wiki.forum.nokia.com/index.php/Graphics_memory_handling
            case EEventUser: {
                TApaSystemEvent *eventData =
                        reinterpret_cast<TApaSystemEvent*>(event->EventData());

                if ((*eventData) == EApaSystemEventShutdown) {
                    qDebug() << "EApaSystemEventShutdown";
                    eventData++;

                    if ((*eventData) == KGoomMemoryLowEvent) {
                        qDebug() << "KGoomMemoryLowEvent";
                        return true;
                    }
                }
            break;
            }
            default:
                break;
        }

        // Always return false so we don't stop the event from being processed
        return false;
    }
#endif
};


int main(int argc, char *argv[])
{
#ifdef Q_OS_HARMATTAN
    QApplication::setGraphicsSystem("raster");
#endif

    MyApplication app(argc, argv);
    qmlRegisterType<MirrorItem>("CustomItems", 1, 0, "MirrorItem");

    // Lock Symbian orientation
#ifdef Q_OS_SYMBIAN
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAP_IGNORE( 
    if (appUi) {
        appUi->SetOrientationL(CAknAppUi::EAppUiOrientationPortrait);
    }
    );
#endif

    QDeclarativeView* view = new QDeclarativeView();

#ifdef Q_OS_HARMATTAN
    view->rootContext()->setContextProperty("isHarmattan", true);
#else
    view->rootContext()->setContextProperty("isHarmattan", false);
#endif

    view->setSource(QUrl("qrc:/main.qml"));
    view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    QObject::connect((QObject*)view->engine(), SIGNAL(quit()), &app, SLOT(quit()));
    view->showFullScreen();

    int ret = app.exec();
    delete view;
    return ret;
}
