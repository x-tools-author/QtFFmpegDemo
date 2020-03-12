#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QImage>
#include <QQuickImageProvider>

class ImageProvider:public QQuickImageProvider
{
public:
    ImageProvider(ImageType type = QQmlImageProviderBase::Image);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

    void setImage(QImage image){_image = image;}
private:
    QImage _image;
};

#endif
