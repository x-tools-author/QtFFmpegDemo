#ifndef VIDEOOUTPUTSOURCE_HH
#define VIDEOOUTPUTSOURCE_HH

#include <QObject>
#include <QAbstractVideoSurface>

class VideoOutputSource : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface NOTIFY videoSurfaceChanged)
public:
    VideoOutputSource(QObject *parent = Q_NULLPTR);
private:
    QAbstractVideoSurface *_videoSurface;
    QAbstractVideoSurface *videoSurface();
    void setVideoSurface(QAbstractVideoSurface* surface);
signals:
    void videoSurfaceChanged();
};

#endif
