#include <QPainter>
#include <QQmlEngine>

#include "Decoder.hpp"
#include "Application.hpp"
#include "QuickPaintedItem.hpp"

QuickPaintedItem::QuickPaintedItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::JavaScriptOwnership);
    Decoder *decoder = reinterpret_cast<Application*>(qApp)->decoderInstance();
    if (decoder){
        connect(decoder, &Decoder::imageChanged, this, &QuickPaintedItem::setImage);
    }
}

void QuickPaintedItem::paint(QPainter *painter)
{
    if (videoFrame.isNull()){
        return;
    }

    /// @brief 等比例缩放绘制
    QPixmap pixmap = QPixmap::fromImage(videoFrame);
    if (this->width() > this->height()){
        pixmap = pixmap.scaledToHeight(static_cast<int>(height()));
    }else {
        pixmap = pixmap.scaledToWidth(static_cast<int>(width()));
    }
    painter->drawPixmap(QRect(static_cast<int>((width()-pixmap.width())/2),
                              static_cast<int>((height()-pixmap.height())/2),
                              pixmap.width(),
                              pixmap.height()),
                        QPixmap::fromImage(videoFrame));
}

void QuickPaintedItem::setImage(QImage image)
{
    videoFrame = image;
    update();
}
