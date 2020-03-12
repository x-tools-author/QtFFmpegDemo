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
    /**
     * 当前解码输出宽度
     */
    int rgbWidth = 0;

    /**
     * 当前解码输出高度
     */
    int rgbHeight = 0;

    /**
     * rgb数据存储地址
     */
    uint8_t* rgbData[4];

    /**
     * 并不知道这个是什么,参考了ffmpeg 官方实例
     */
    int rgbLinesize[4];
private:
    int decode_write(AVCodecContext *avctx, AVPacket *packet);
    void outputImage(AVCodecContext *avctx, AVFrame *frame);
    void saveImageToList(uint8_t *data, int width, int height, QImage::Format format);
signals:

};

#endif // DECODER_HPP
