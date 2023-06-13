#include "CustomSource.h"
#include <QVideoFrameFormat>

#include <QOpenGLContext>
#include <QRandomGenerator>
#include <QPainter>

CustomSource::CustomSource(QQuickItem* parent): QQuickItem(parent)
{
	m_timer.setInterval(33);
    avdevice_register_all();

    const AVInputFormat* iformat = av_find_input_format("dshow");
    connect(this, &QQuickItem::windowChanged, this, &CustomSource::setWindow);

	connect(&m_timer, &QTimer::timeout, this, &CustomSource::handleTimeout);
    
    int exitStatus = 0;
    const std::string path = "video=Virtual Camera";
    if (avformat_open_input(&pFormatCtx, path.c_str(), iformat, NULL) != 0) {
        exitStatus = 1;
    }
    av_dump_format(pFormatCtx, 0, path.c_str(), 0);
    
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIdx = i;
            break;

        }
    }
    if (videoStreamIdx == -1) {
        exitStatus = 1;
    }


    const AVCodec* pCodec = avcodec_find_decoder(pFormatCtx->streams[videoStreamIdx]->codecpar->codec_id);

    if (pCodec == nullptr) {
        exitStatus = 1;
    }

    pCodecCtxOrig = avcodec_alloc_context3(pCodec);
    auto ret = avcodec_parameters_to_context(pCodecCtxOrig, pFormatCtx->streams[videoStreamIdx]->codecpar);
    if (ret < 0) {
        exitStatus = 1;
    }


    if (avcodec_open2(pCodecCtxOrig, pCodec, nullptr) != 0) {
        exitStatus = 1;
    }

     dx = pCodecCtxOrig->width;
     dy = pCodecCtxOrig->height;

    pFrame = av_frame_alloc();
    decodedFrame = av_frame_alloc();

    int format = (pFormatCtx->streams[0]->codecpar->format);

    if (format == -1) {
        format = AV_PIX_FMT_YUV420P;
    }

    sws_ctx = sws_getContext(pCodecCtxOrig->width,
        pCodecCtxOrig->height,
        static_cast<AVPixelFormat>(format),
        dx,
        dy,
        AV_PIX_FMT_YUYV422,
        SWS_BILINEAR,
        NULL,
        NULL,
        NULL
    );



    ready = true;

}

void CustomSource::start()
{
	m_timer.start();
	handleTimeout();
    video_frame = (QVideoFrameFormat(QSize(dx, dy), QVideoFrameFormat::Format_YUYV));
    if (!video_frame.isValid() || !video_frame.map(QVideoFrame::WriteOnly)) {
        qWarning() << "QVideoFrame is not valid or not writable";
        return;
    }
}

void CustomSource::handleTimeout() 
{
	
    

    constexpr int plane = 0;


    av_image_fill_arrays(decodedFrame->data,
        decodedFrame->linesize, video_frame.bits(plane), AV_PIX_FMT_YUYV422, dx,
        dy, 2);


    if (av_read_frame(pFormatCtx, &packet) >= 0) {
        if (packet.stream_index == videoStreamIdx) {
            avcodec_send_packet(pCodecCtxOrig, &packet);
            if (avcodec_receive_frame(pCodecCtxOrig, pFrame) >= 0) {
                sws_scale(sws_ctx, (uint8_t const* const*)pFrame->data,
                    pFrame->linesize, 0, pCodecCtxOrig->height,
                    decodedFrame->data, decodedFrame->linesize);



                av_packet_unref(&packet);
            }

        }
    }

    video_frame.unmap();

    m_videoSink->setVideoFrame(video_frame);


}

void CustomSource::sync()
{
    {
        QMutexLocker locker(&mutex);
        if (!ready)
        {
            return;
        }
        ready = false;
    }


    if (!m_videoSink)
        return;

    ++idx;
    if (idx >= frames.size())
    {
        idx = 0;
    }
	
       ready = true;
}
