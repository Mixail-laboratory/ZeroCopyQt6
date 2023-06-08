#pragma once

#include <QQuickItem>
#include <QString>
#include <QVideoSink>
#include <QMutex>
#include <QTimer>
#include <QVideoFrame>
#include <vector>



class CustomSource : public QObject
{
    Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(QVideoSink* videoSink READ videoSink WRITE setVideoSink NOTIFY videoSinkChanged)
public:
    explicit CustomSource(QObject* parent = nullptr);
    inline [[nodiscard]] QVideoSink* videoSink() const
    {
        return m_videoSink;
    };
    inline void setVideoSink(QVideoSink* newVideoSink)
    {
        m_videoSink = newVideoSink;
        emit videoSinkChanged();
    };
    Q_INVOKABLE void start();
    ~CustomSource() override = default;
signals:
    void videoSinkChanged();
private:
    QPointer<QVideoSink> m_videoSink;
    void handleTimeout() ;
    QTimer m_timer;

    std::vector<QVideoFrame> frames;
    int idx = 0;
};

