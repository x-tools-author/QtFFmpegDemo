#include "Decoder.hpp"
#include "Application.hpp"
#include "ImageProvider.hpp"

ImageProvider::ImageProvider(ImageType type)
    :QQuickImageProvider (type)
{
    _image = QImage(":/resources/F.png");

    Decoder *decoder = reinterpret_cast<Application*>(qApp)->decoderInstance();
    if (decoder){
        QObject::connect(decoder, &Decoder::imageChanged, qApp, [&](QImage image){
            setImage(image);
        });
    }
}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    Q_UNUSED(requestedSize);

    size->setWidth(_image.width());
    size->setHeight(_image.height());

    return _image;
}
