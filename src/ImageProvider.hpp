/**************************************************************************
 * The file is encoding with utf-8 (with BOM).
 * The file is part of "4g link data transmit tool" project.
 *
 * Copyright (C), 2018-2019. 广州雷迅创新科技有限公司，all rights reserve.
 *************************************************************************/
#ifndef TTVIDEOIMAGEPROVIDER_H
#define TTVIDEOIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>

class VideoImageProvider:public QQuickImageProvider
{
public:
    VideoImageProvider(ImageType type = QQmlImageProviderBase::Image);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

    void setImage(QImage iamge){_image = iamge;}
private:
    QImage _image;
};

#endif
