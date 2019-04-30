#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "filemanager.h"
#include "utisomanager.h"
#include "genericisomanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setSetuidAllowed(true);
    qmlRegisterType<FileManager>("FileManager", 1, 0, "FileManager");
    qmlRegisterType<UtIsoManager>("ISOManager", 1, 0, "ISOManager");

    FileManager* fileManager = new FileManager();
    UtIsoManager* isoManager = new UtIsoManager();

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication* app = new QGuiApplication(argc, argv);
    QQmlApplicationEngine* engine = new QQmlApplicationEngine(app);

    engine->rootContext()->setContextProperty("fileManager", fileManager);
    engine->rootContext()->setContextProperty("isoManager", isoManager);
    engine->load(QStringLiteral("qrc:/qml/main.qml"));

    return app->exec();
}

