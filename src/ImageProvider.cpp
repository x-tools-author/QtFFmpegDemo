/**************************************************************************
 * The file is encoding with utf-8 (with BOM).
 * The file is part of "4g link data transmit tool" project.
 *
 * Copyright (C), 2018-2019. 广州雷迅创新科技有限公司，all rights reserve.
 *************************************************************************/
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "VideoImageProvider.hpp"

VideoImageProvider::VideoImageProvider(ImageType type)
    :QQuickImageProvider (type)
{
    _image = QImage(":/images/NoVideo.png");
}

QImage VideoImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    Q_UNUSED(requestedSize);

    size->setWidth(_image.width());
    size->setHeight(_image.height());

    return _image;
}
