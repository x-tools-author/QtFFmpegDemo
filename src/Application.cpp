#include <QDebug>
#include <QDateTime>

#include "Decoder.hpp"
#include "Application.hpp"
#include "ImageProvider.hpp"

Application::Application(int argc, char **argv)
    : QGuiApplication(argc, argv)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    decoder = new Decoder;
    imageProvider = new ImageProvider;

    connect(decoder, &Decoder::finished, this, [&](){
        decoder->deleteLater();
        decoder = nullptr;
    });

    connect(decoder, &Decoder::imageChanged, this, [&](QImage img){
        imageProvider->setImage(img);
    });

    engine.addImageProvider(QString("videoprovider"), imageProvider);
    decoder->start();
    registerCustomType();
}

Application::~Application()
{
    if (decoder){
        decoder->requestInterruption();
    }
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

void Application::registerCustomType()
{
    qmlRegisterSingletonType<Application>("CustomType", 1, 0, "Application", [](QQmlEngine*, QJSEngine*)->QObject*{
        return reinterpret_cast<Application*>(qApp);
    });
}
