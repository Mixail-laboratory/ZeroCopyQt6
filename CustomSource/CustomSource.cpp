#include "CustomSource.h"
#include <QQuickWindow>
#include <QVideoFrameFormat>

#include <QOpenGLContext>
#include <QRandomGenerator>
#include <QDateTime>
#include <QPainter>

CustomSource::CustomSource(QObject* parent)
{
	m_timer.setInterval(3);
	connect(&m_timer, &QTimer::timeout, this, &CustomSource::handleTimeout);

    for (int i = 0; i < 10; i++)
    {
        QVideoFrame video_frame(QVideoFrameFormat(QSize(640, 480), QVideoFrameFormat::Format_BGRA8888));
        if (!video_frame.isValid() || !video_frame.map(QVideoFrame::WriteOnly)) {
            qWarning() << "QVideoFrame is not valid or not writable";
            return;
        }
        const QImage::Format image_format = QVideoFrameFormat::imageFormatFromPixelFormat(video_frame.pixelFormat());
        if (image_format == QImage::Format_Invalid) {
            qWarning() << "It is not possible to obtain image format from the pixel format of the videoframe";
            return;
        }
        constexpr int plane = 0;
        QImage image(video_frame.bits(plane), video_frame.width(), video_frame.height(), image_format);
        image.fill(QColor::fromRgb(QRandomGenerator::global()->generate()));
        QPainter painter(&image);
        painter.end();

        video_frame.unmap();
        frames.push_back(video_frame);
	    
    }
}

void CustomSource::start()
{
	m_timer.start();
	handleTimeout();
}

void CustomSource::handleTimeout() 
{
    if (!m_videoSink)
        return;
	
    m_videoSink->setVideoFrame(frames[idx]);
    ++idx;
    if (idx >= frames.size())
    {
        idx = 0;
    }
}


