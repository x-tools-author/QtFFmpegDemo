#ifndef DECODER_HPP
#define DECODER_HPP

#include <QImage>
#include <QThread>

extern "C" {
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/avassert.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/hwcontext.h>
#include <libavformat/avformat.h>
}

class Decoder : public QThread
{
    Q_OBJECT
public:
    explicit Decoder(QObject *parent = nullptr);
protected:
    void run();
private:
    /// @brief 解码相关参数
    int rgbWidth = 0;
    int rgbHeight = 0;
    uint8_t* rgbData[4];
    int rgbLinesize[4];
private:
    int decodeActually(AVCodecContext *avctx, AVPacket *packet);
    void outputImage(AVCodecContext *avctx, AVFrame *frame);
    void saveImageToList(uint8_t *data, int width, int height, QImage::Format format);
signals:
    void imageChanged(QImage image);
};

#endif // DECODER_HPP
