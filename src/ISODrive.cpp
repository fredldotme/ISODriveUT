#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

#include "common.h"
#include "filemanager.h"
#include "utisomanager.h"
#include "configfsisomanager.h"
#include "genericisomanager.h"

enum UsbType {
    ANDROID0,
    CONFIGFS
};

static UsbType getUsbGadgetType()
{
    const UsbType defaultType = ANDROID0;
    bool hasConfigFs = false;

    QFile filesystems(QStringLiteral("/proc/filesystems"));
    if (!filesystems.open(QFile::ReadOnly)) {
        qWarning() << "Failed to open /proc/filesystems";
        return defaultType;
    }

    while (true) {
        const QByteArray line = filesystems.readLine();
        const QString lineString = QString(line).trimmed();
        if (lineString.isEmpty())
            break;

        qDebug() << lineString;
        const QStringList lineSplit = lineString.split('\t', QString::SkipEmptyParts);
        if (lineSplit.size() < 2)
            continue;

        if (lineSplit[0] == QStringLiteral("nodev") &&
                lineSplit[1] == QStringLiteral("configfs")) {
            hasConfigFs = true;
            break;
        }
    }

    if (hasConfigFs && !QFile(ANDROID0_SYSFS_IMG_FILE).exists())
        return CONFIGFS;

    return defaultType;
}

int main(int argc, char *argv[])
{
    qmlRegisterType<FileManager>("FileManager", 1, 0, "FileManager");
    qmlRegisterType<UtIsoManager>("ISOManager", 1, 0, "ISOManager");
    qmlRegisterType<ConfigFSIsoManager>("ConfigFsISOManager", 1, 0, "ConfigFsISOManager");

    FileManager* fileManager = new FileManager();
    GenericIsoManager* isoManager = Q_NULLPTR;
    const UsbType usbType = getUsbGadgetType();

    switch(usbType) {
    case ANDROID0:
        qDebug() << "Selecting UtIsoManager";
        isoManager = new UtIsoManager();
        break;
    case CONFIGFS:
        qDebug() << "Selecting ConfigFSIsoManager";
        isoManager = new ConfigFSIsoManager();
        break;
    default:
        qWarning() << "UsbType not supported" << usbType;
        break;
    }

#if 0
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
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

    engine->rootContext()->setContextProperty("hasSettings", usbType == ANDROID0);
    engine->rootContext()->setContextProperty("fileManager", fileManager);
    engine->rootContext()->setContextProperty("isoManager", isoManager);
    engine->load(QStringLiteral("qrc:/qml/main.qml"));

    return app->exec();
}

