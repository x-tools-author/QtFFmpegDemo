#include <QDebug>
#include <QDateTime>

#include "Decoder.hpp"
#include "Application.hpp"
#include "ImageProvider.hpp"
#include "QuickPaintedItem.hpp"
#include "VideoOutputSource.hpp"

Application::Application(int argc, char **argv)
    : QGuiApplication(argc, argv)
{
    decoder = new Decoder;
    connect(decoder, &Decoder::finished, this, [&](){
        decoder->deleteLater();
        decoder = nullptr;
    });

    engine.addImageProvider(QString("videoprovider"), new ImageProvider);
    decoder->start();

    qmlRegisterType<QuickPaintedItem>("CustomType", 1, 0, "QuickPaintedItem");
    qmlRegisterType<VideoOutputSource>("CustomType", 1, 0, "VideoOutputSource");
}

Application::~Application()
{
    if (decoder){
        decoder->requestInterruption();
    }
}

Decoder *Application::decoderInstance()
{
    return decoder;
}

void Application::setupUi()
{
    const QUrl url(QStringLiteral("qrc:/ui/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     this, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
}
