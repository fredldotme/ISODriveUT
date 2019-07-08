#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

#include "filemanager.h"
#include "utisomanager.h"
#include "genericisomanager.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<FileManager>("FileManager", 1, 0, "FileManager");
    qmlRegisterType<UtIsoManager>("ISOManager", 1, 0, "ISOManager");
    FileManager* fileManager = new FileManager();
    UtIsoManager* isoManager = new UtIsoManager();

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication* app = new QGuiApplication(argc, argv);
    app->setApplicationName(QStringLiteral("me.fredl.isodrive"));
    QQmlApplicationEngine* engine = new QQmlApplicationEngine(app);

    const QByteArray language = qgetenv("LANGUAGE");
    const QString translationDir = app->applicationDirPath() + QStringLiteral("/i18n/");
    QTranslator* translator = new QTranslator(app);
    const bool hasTranslation =
            translator->load(QStringLiteral("isodrive-%1").arg(QString::fromUtf8(language)),
                             translationDir);
    if (hasTranslation)
        app->installTranslator(translator);

    engine->rootContext()->setContextProperty("fileManager", fileManager);
    engine->rootContext()->setContextProperty("isoManager", isoManager);
    engine->load(QStringLiteral("qrc:/qml/main.qml"));

    return app->exec();
}

