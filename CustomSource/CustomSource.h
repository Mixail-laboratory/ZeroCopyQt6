#pragma once

#include <QQuickItem>
#include <QString>
#include <QVideoSink>
#include <QMutex>
#include <QTimer>
#include <QVideoFrame>
#include <QQuickWindow>
#include <vector>
#include <future>

extern "C"
{

#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavformat/avio.h>
#include <libavutil/pixdesc.h>
#include <libavutil/hwcontext.h>
#include <libavutil/opt.h>
#include <libavutil/avassert.h>
#include <libavutil/imgutils.h>
#include <libavutil/motion_vector.h>
#include <libavutil/frame.h>
}



#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "swresample.lib")

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

// compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif





class CustomSource : public QQuickItem
{
    Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged)
    Q_PROPERTY(QString device MEMBER m_device READ device WRITE setDevice)
public:
    explicit CustomSource(QQuickItem* parent = nullptr);

    inline [[nodiscard]] QVideoSink* videoSink() const
    {
        return m_videoSink;
    }

    inline void setVideoSink(QVideoSink* newVideoSink)
    {
        m_videoSink = newVideoSink;
        emit videoSinkChanged();
    }

    inline void setDevice(const QString& device)
    {
        m_device = device;
    }

    [[nodiscard]] QString device() const
    {
        return m_device;
    }

    Q_INVOKABLE void start();
    ~CustomSource() override = default;

signals:
    void videoSinkChanged();


private slots:
    void setWindow(QQuickWindow* win)
    {
        if (win)
        {
            connect(win, &QQuickWindow::beforeSynchronizing, this,
                &CustomSource::sync, Qt::DirectConnection);
        }
    }

    void sync();
	

private:
    QPointer<QVideoSink> m_videoSink;
    void handleTimeout() ;
    QTimer m_timer;

    std::vector<QVideoFrame> frames;
    QString m_device{};
    int idx = 0;

    QMutex mutex;
    bool ready;
    std::future<void> worker_handle;
    QVideoFrame video_frame;
private:
    AVPacket packet;
    AVFormatContext* pFormatCtx = nullptr;
    AVCodecContext* pCodecCtxOrig = nullptr;
    AVFrame* pFrame = nullptr;
    AVFrame* decodedFrame = nullptr;
    struct SwsContext* sws_ctx;
    int dx{};
    int dy{};
    int videoStreamIdx = -1;
};

