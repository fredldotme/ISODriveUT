#include "utisomanager.h"

#include <QDBusReply>
#include <QDebug>
#include <QFile>

const QString SYSFS_ENABLE =
        QStringLiteral("/sys/devices/virtual/android_usb/android0/enable");
const QString SYSFS_FEATURES =
        QStringLiteral("/sys/devices/virtual/android_usb/android0/functions");
const QString SYSFS_IMG_FILE =
        QStringLiteral("/sys/devices/virtual/android_usb/android0/f_mass_storage/lun/file");

const QString PROPERTY_SERVICE_PATH =
        QStringLiteral("/com/canonical/PropertyService");
const QString PROPERTY_SERVICE_OBJ =
        QStringLiteral("com.canonical.PropertyService");

UtIsoManager::UtIsoManager(QObject *parent) :
    GenericIsoManager(parent),
    m_commandRunner(new CommandRunner(this)),
    m_propertyService(new QDBusInterface(PROPERTY_SERVICE_OBJ,
                                         PROPERTY_SERVICE_PATH,
                                         PROPERTY_SERVICE_OBJ,
                                         QDBusConnection::systemBus(),
                                         this))
{
    connect(this->m_commandRunner, &CommandRunner::passwordRequested,
            this->m_commandRunner, [=]() {
        qDebug() << "passwd requested";
        this->m_commandRunner->providePassword(this->userPassword());
    }, Qt::DirectConnection);
}

void UtIsoManager::setUserPassword(const QString &password)
{
    if (this->m_userPassword == password)
        return;

    this->m_userPassword = password;
    emit userPasswordChanged();
}

QString UtIsoManager::userPassword()
{
    return this->m_userPassword;
}

bool UtIsoManager::validatePassword()
{
    return this->m_commandRunner->validatePassword();
}

void UtIsoManager::enableISO(const QString& fileName, const bool enableSharing)
{
    if(enabled())
        setEnabled(false);

    const QByteArray selectedIso = fileName.toUtf8();
    this->m_commandRunner->writeFile(SYSFS_IMG_FILE, selectedIso);
    emit selectedISOChanged();

    QByteArray features = "mass_storage";
    if (enableSharing) {
        features += ",mtp";
    }
    features += "\n";
    this->m_commandRunner->writeFile(SYSFS_FEATURES, features);

    const QByteArray finalSelectedIso = this->m_commandRunner->readFile(SYSFS_IMG_FILE);
    if(selectedIso != finalSelectedIso) {
        emit selectionFailed();
    }

    setEnabled(true);
}

bool UtIsoManager::isEnabledISO(const QString absolutePath)
{
    return (absolutePath.compare(getSelectedISOPath(), Qt::CaseInsensitive) == 0);
}

void UtIsoManager::resetISO()
{
    enableISO("");
    if(enabled())
        setEnabled(false);

    QDBusReply<bool> reply = m_propertyService->call(QStringLiteral("GetProperty"),
                                                     QStringLiteral("adb"));
    QByteArray features = QByteArrayLiteral("mtp");
    if (reply.isValid() && reply.value()) {
        features += QByteArrayLiteral(",adb");
    }
    features += "\n";

    this->m_commandRunner->writeFile(SYSFS_FEATURES, features);
    setEnabled(true);
}

bool UtIsoManager::enabled()
{
    const QString content = this->m_commandRunner->readFile(SYSFS_ENABLE);
    const bool ret = content.startsWith("1");
    return ret;
}

void UtIsoManager::setEnabled(bool enabled)
{
    this->m_commandRunner->writeFile(SYSFS_ENABLE, enabled ? "1\n" : "0\n");
}

QString UtIsoManager::getSelectedISOPath()
{
    const QByteArray path = this->m_commandRunner->readFile(SYSFS_IMG_FILE);
    QString content = QString::fromUtf8(path);
    QString fileName = content;
    fileName = fileName.replace("\n", "");
    return fileName.trimmed();
}

QString UtIsoManager::getSelectedISO()
{
    const QString content = getSelectedISOPath();
    QString fileName = content.mid(content.lastIndexOf("/") + 1);
    fileName = fileName.replace("\n", "");
    return fileName.trimmed();
}
