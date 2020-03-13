#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>

#include "Decoder.hpp"

static AVBufferRef *hw_device_ctx = NULL;
static enum AVPixelFormat hw_pix_fmt;

static int hw_decoder_init(AVCodecContext *ctx, const enum AVHWDeviceType type)
{
    int err = 0;

    if ((err = av_hwdevice_ctx_create(&hw_device_ctx, type,
                                      NULL, NULL, 0)) < 0) {
        fprintf(stderr, "Failed to create specified HW device.\n");
        return err;
    }
    ctx->hw_device_ctx = av_buffer_ref(hw_device_ctx);

    return err;
}

static enum AVPixelFormat get_hw_format(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts)
{
    Q_UNUSED(ctx);
    const enum AVPixelFormat *p;

    for (p = pix_fmts; *p != -1; p++) {
        if (*p == hw_pix_fmt)
            return *p;
    }

    fprintf(stderr, "Failed to get HW surface format.\n");
    return AV_PIX_FMT_NONE;
}

int Decoder::decodeActually(AVCodecContext *avctx, AVPacket *packet)
{
    AVFrame *frame = NULL, *sw_frame = NULL;
    AVFrame *tmp_frame = NULL;
    uint8_t *buffer = NULL;
    int ret = 0;

    ret = avcodec_send_packet(avctx, packet);
    if (ret < 0) {
        fprintf(stderr, "Error during decoding\n");
        return ret;
    }

    while (1) {
        if (!(frame = av_frame_alloc()) || !(sw_frame = av_frame_alloc())) {
            fprintf(stderr, "Can not alloc frame\n");
            ret = AVERROR(ENOMEM);
            goto fail;
        }

        ret = avcodec_receive_frame(avctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            av_frame_free(&frame);
            av_frame_free(&sw_frame);
            return 0;
        } else if (ret < 0) {
            fprintf(stderr, "Error while decoding\n");
            goto fail;
        }

        if (frame->format == hw_pix_fmt) {
            /* retrieve data from GPU to CPU */
            if ((ret = av_hwframe_transfer_data(sw_frame, frame, 0)) < 0) {
                fprintf(stderr, "Error transferring the data to system memory\n");
                goto fail;
            }
            tmp_frame = sw_frame;
        } else{
            tmp_frame = frame;
        }

        outputImage(avctx, tmp_frame);
#if 0
        size = av_image_get_buffer_size(static_cast<enum AVPixelFormat>(tmp_frame->format), tmp_frame->width, tmp_frame->height, 1);
        buffer = reinterpret_cast<uint8_t*>(av_malloc(size));
        if (!buffer) {
            fprintf(stderr, "Can not alloc buffer\n");
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        ret = av_image_copy_to_buffer(buffer, size,
                                      (const uint8_t * const *)tmp_frame->data,
                                      (const int *)tmp_frame->linesize, static_cast<enum AVPixelFormat>(tmp_frame->format),
                                      tmp_frame->width, tmp_frame->height, 1);
        if (ret < 0) {
            fprintf(stderr, "Can not copy image to buffer\n");
            goto fail;
        }
#endif


    fail:
        av_frame_free(&frame);
        av_frame_free(&sw_frame);
        av_freep(&buffer);
        if (ret < 0)
            return ret;
    }
}

Decoder::Decoder(QObject *parent)
    : QThread(parent)
{
    rgbData[0] = nullptr;
    rgbData[1] = nullptr;
    rgbData[2] = nullptr;
    rgbData[3] = nullptr;
}

void Decoder::run()
{
    AVFormatContext *input_ctx = NULL;
    int video_stream, ret;
    AVStream *video = NULL;
    AVCodecContext *decoder_ctx = NULL;
    AVCodec *decoder = NULL;
    AVPacket packet;
    int i;

    enum AVHWDeviceType type = av_hwdevice_find_type_by_name("dxva2");
    if (type == AV_HWDEVICE_TYPE_NONE) {
        fprintf(stderr, "Device type %d is not supported.\n", type);
        fprintf(stderr, "Available device types:");
        while((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE)
            fprintf(stderr, " %s", av_hwdevice_get_type_name(type));
        fprintf(stderr, "\n");
        return;
    }

    /* open the input file */
    if (avformat_open_input(&input_ctx, QCoreApplication::applicationDirPath().append("/Hi3516A_1080P30fps_800kbps_0626.h265").toLatin1().data(), NULL, NULL) != 0) {
        fprintf(stderr, "Cannot open input file '%s'\n", QCoreApplication::applicationDirPath().append("/Hi3516A_1080P30fps_800kbps_0626.h265").toLatin1().data());
        return;
    }else{
        qInfo() << __FUNCTION__ << __LINE__ << QCoreApplication::applicationDirPath().append("/Hi3516A_1080P30fps_800kbps_0626.h265").toLatin1().data();
    }

    if (avformat_find_stream_info(input_ctx, NULL) < 0) {
        fprintf(stderr, "Cannot find input stream information.\n");
        return;
    }

    /* find the video stream information */
    ret = av_find_best_stream(input_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
    if (ret < 0) {
        fprintf(stderr, "Cannot find a video stream in the input file\n");
        return;
    }
    video_stream = ret;

    for (i = 0; ; i++) {
        const AVCodecHWConfig *config = avcodec_get_hw_config(decoder, i);
        if (!config) {
            fprintf(stderr, "Decoder %s does not support device type %s.\n",
                    decoder->name, av_hwdevice_get_type_name(type));
            return;
        }
        if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
            config->device_type == type) {
            hw_pix_fmt = config->pix_fmt;
            break;
        }
    }

    if (!(decoder_ctx = avcodec_alloc_context3(decoder))){
        qWarning() << "Can not alloc context for decoder" <<  AVERROR(ENOMEM);
        return;
    }

    video = input_ctx->streams[video_stream];
    if (avcodec_parameters_to_context(decoder_ctx, video->codecpar) < 0){
        qWarning() << "Parameters to context failed";
        return;
    }

    decoder_ctx->get_format  = get_hw_format;
    av_opt_set_int(decoder_ctx, "refcounted_frames", 1, 0);

    if (hw_decoder_init(decoder_ctx, type) < 0){
        qWarning() << "Can not init decoder";
        return;
    }

    if ((ret = avcodec_open2(decoder_ctx, decoder, NULL)) < 0) {
        fprintf(stderr, "Failed to open codec for stream #%u\n", video_stream);
        return;
    }

    /* actual decoding and dump the raw data */
    while (ret >= 0) {
        quint64 t1 = QDateTime::currentDateTime().toMSecsSinceEpoch();
        if (isInterruptionRequested()){
            break;
        }

        if ((ret = av_read_frame(input_ctx, &packet)) < 0){
            break;
        }

        if (video_stream == packet.stream_index){
            ret = decodeActually(decoder_ctx, &packet);
        }

        av_packet_unref(&packet);
        quint64 t2 = QDateTime::currentDateTime().toMSecsSinceEpoch();
        if ((t2 - t1) < (1000/30)){
            msleep(t2 - t1);
        }
    }

    /* flush the decoder */
    packet.data = NULL;
    packet.size = 0;
    ret = decodeActually(decoder_ctx, &packet);
    av_packet_unref(&packet);

    avcodec_free_context(&decoder_ctx);
    avformat_close_input(&input_ctx);
    av_buffer_unref(&hw_device_ctx);
}

void Decoder::outputImage(AVCodecContext *avctx, AVFrame *frame)
{
    int ret = 0;

    if ((avctx->width <= 0) || (avctx->height <= 0)) {
        return;
    }

    /// @brief 分辨率发生改变，重新分配内存
    if ((rgbWidth != avctx->width) || (rgbHeight != avctx->height)){
        rgbWidth = avctx->width;
        rgbHeight = avctx->height;

        if (rgbData[0]){
            av_freep(&rgbData[0]);

            for (int i = 0; i < 4; i++){
                rgbData[i] = nullptr;
            }
        }

        /// @brief 申请的内存在析构函数中释放
        ret = av_image_alloc(rgbData, rgbLinesize, avctx->width, avctx->height, AV_PIX_FMT_RGB32, 1);
        if (ret <= 0){
            av_freep(&rgbData[0]);
            return;
        }
    }

    SwsContext *swsContext = nullptr;
    swsContext = sws_getContext(avctx->width,
                                avctx->height,
                                static_cast<AVPixelFormat>(frame->format),
                                avctx->width,
                                avctx->height,
                                AV_PIX_FMT_RGB32,
                                SWS_BICUBIC,
                                nullptr,
                                nullptr,
                                nullptr);

    if (!swsContext) {
         return;
    }

    /// @brief 格式转换，生成图像
    ret = sws_scale(swsContext, frame->data, frame->linesize, 0, avctx->height, rgbData, rgbLinesize);
    if (ret > 0) {
#if 0
        saveImageToList(rgbData[0], avctx->width, avctx->height, QImage::Format_RGB32);
#else
        QImage *image = new QImage(QSize(avctx->width, avctx->height), QImage::Format_RGB32);
        memcpy(image->bits(), rgbData[0], static_cast<size_t>(image->sizeInBytes()));
        emit imageChanged(*image);
        delete image;
#endif
    }

    /// @brief 内存释放
    sws_freeContext(swsContext);
}

void Decoder::saveImageToList(uint8_t *data, int width, int height, QImage::Format format)
{
    QImage *image = new QImage(QSize(width, height), format);
    memcpy(image->bits(), data, static_cast<size_t>(image->sizeInBytes()));
    QString fileName = QDateTime::currentDateTime().toString("hhmmsszzz").append(".jpg").prepend("");
    if (image->save(fileName, "JPG", 100)){
        requestInterruption();
    }else{
        qWarning() << "can not save image to file" << image->size() << image->sizeInBytes();
    }
    delete image;
}
