#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "CustomSource.h"
#include <QQmlEngine>


int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
		
    QQmlApplicationEngine engine;
    qmlRegisterType<CustomSource>("CustomSource", 1, 0, "CameraSource");
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
	engine.load(url);


	if (engine.rootObjects().isEmpty())
        return -1;



return app.exec();
}
