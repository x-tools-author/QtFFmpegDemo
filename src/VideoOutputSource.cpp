#include <QDebug>
#include <QQmlEngine>
#include <QVideoSurfaceFormat>

#include "Decoder.hpp"
#include "Application.hpp"
#include "VideoOutputSource.hpp"

VideoOutputSource::VideoOutputSource(QObject *parent)
    : QObject(parent)
    , _videoSurface(Q_NULLPTR)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::JavaScriptOwnership);

    Decoder *decoder = reinterpret_cast<Application*>(qApp)->decoderInstance();
    if (decoder){
        connect(decoder, &Decoder::imageChanged, this, [&](QImage image){
            _videoSurface->present(QVideoFrame(image));
            _videoSurface->start(QVideoSurfaceFormat(QSize(image.width(), image.height()), QVideoFrame::Format_BGRA32));
        });
    }
}

QAbstractVideoSurface* VideoOutputSource::videoSurface()
{
    return _videoSurface;
}

void VideoOutputSource::setVideoSurface(QAbstractVideoSurface *surface)
{
    QAbstractVideoSurface *old = _videoSurface;
    _videoSurface = surface;
    if (old != _videoSurface){
        emit videoSurfaceChanged();
    }
}
