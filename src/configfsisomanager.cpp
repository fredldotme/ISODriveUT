#include "configfsisomanager.h"
#include "common.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QDBusReply>
#include <QFileInfo>

ConfigFSIsoManager::ConfigFSIsoManager(QObject *parent) :
    GenericIsoManager(parent),
    m_commandRunner(new CommandRunner(this))
{
    connect(this->m_commandRunner, &CommandRunner::passwordRequested,
            this->m_commandRunner, [=]() {
        qDebug() << "passwd requested";
        this->m_commandRunner->providePassword(this->userPassword());
    }, Qt::DirectConnection);
}

void ConfigFSIsoManager::setUserPassword(const QString &password)
{
    if (this->m_userPassword == password)
        return;

    this->m_userPassword = password;
    emit userPasswordChanged();
}

QString ConfigFSIsoManager::userPassword()
{
    return this->m_userPassword;
}

bool ConfigFSIsoManager::validatePassword()
{
    return this->m_commandRunner->validatePassword();
}

QString ConfigFSIsoManager::getGadgetRoot()
{
    const QString configFsRoot = this->m_commandRunner->getConfigFsMountPoint();
    const QString usbGadgetRoot = configFsRoot + QStringLiteral("/usb_gadget");
    QDirIterator usbGadgetRootIterator(usbGadgetRoot, QDir::Dirs | QDir::NoDotAndDotDot);
    QString gadgetRoot;

    while (usbGadgetRootIterator.hasNext()) {
        const QString entry = usbGadgetRootIterator.next();

        qDebug() << entry;
        if (!QFileInfo(entry).isDir())
            continue;

        gadgetRoot = entry;
        break;
    }

    return gadgetRoot;
}

QString ConfigFSIsoManager::getConfigRoot()
{
    const QString gadgetRoot = getGadgetRoot();
    const QString usbConfigRoot = gadgetRoot + QStringLiteral("/configs");
    QDirIterator usbGadgetRootIterator(usbConfigRoot, QDir::Dirs | QDir::NoDotAndDotDot);
    QString configRoot;

    while (usbGadgetRootIterator.hasNext()) {
        const QString entry = usbGadgetRootIterator.next();

        qDebug() << entry;
        if (!QFileInfo(entry).isDir())
            continue;

        configRoot = entry;
        break;
    }

    qDebug() << "configRoot:" << configRoot;
    return configRoot;
}

void ConfigFSIsoManager::enableISO(const QString& fileName, const bool enableSharing)
{
    Q_UNUSED(enableSharing);

    const QString gadgetRoot = getGadgetRoot();
    const QString configRoot = getConfigRoot();
    const QString functionRoot = gadgetRoot + QStringLiteral("/functions");
    const QString massStorageRoot = functionRoot + QStringLiteral("/mass_storage.0");
    const QString lunRoot = massStorageRoot + QStringLiteral("/lun.0");

    const QString stallFile = massStorageRoot + QStringLiteral("/stall");
    const QString udcFile = gadgetRoot + QStringLiteral("/UDC");
    const QString lunFile = lunRoot + QStringLiteral("/file");
    const QString lunCdRom = lunRoot + QStringLiteral("/cdrom");
    const QString lunRo = lunRoot + QStringLiteral("/ro");

    const QByteArray selectedIso = fileName.toUtf8();

    resetUDC();

    if (!QDir(massStorageRoot).exists()) {
        this->m_commandRunner->mkDir(massStorageRoot);
    }

    if (!QFileInfo().exists(configRoot + QStringLiteral("/mass_storage.0"))) {
        this->m_commandRunner->ln(massStorageRoot,
                                  configRoot + QStringLiteral("/mass_storage.0"));
    }

    this->m_commandRunner->writeFile(lunFile, selectedIso);
    this->m_commandRunner->writeFile(lunCdRom, "0");
    this->m_commandRunner->writeFile(lunRo, "1");

    setUDC();

    emit selectedISOChanged();

    const QByteArray finalSelectedIso = this->m_commandRunner->readFile(lunFile);
    if(selectedIso != finalSelectedIso) {
        emit selectionFailed();
    }
}

void ConfigFSIsoManager::resetUDC()
{
    const QString gadgetRoot = getGadgetRoot();
    const QString udcFile = gadgetRoot + QStringLiteral("/UDC");
    this->m_commandRunner->writeFile(udcFile, "");
}

void ConfigFSIsoManager::setUDC()
{
    const QString gadgetRoot = getGadgetRoot();
    const QString udcFile = gadgetRoot + QStringLiteral("/UDC");
    this->m_commandRunner->writeFile(udcFile,
                                     this->m_commandRunner->getUDCController().toUtf8());
}

bool ConfigFSIsoManager::isEnabledISO(const QString absolutePath)
{
    return (absolutePath.compare(getSelectedISOPath(), Qt::CaseInsensitive) == 0);
}

void ConfigFSIsoManager::resetISO()
{
    enableISO("");
}

QString ConfigFSIsoManager::getSelectedISOPath()
{
    const QString gadgetRoot = getGadgetRoot();
    const QString functionRoot = gadgetRoot + QStringLiteral("/functions");
    const QString massStorageRoot = functionRoot + QStringLiteral("/mass_storage.0");
    const QString lunRoot = massStorageRoot + QStringLiteral("/lun.0");

    const QString lunFile = lunRoot + QStringLiteral("/file");

    const QByteArray path = this->m_commandRunner->readFile(lunFile);
    QString content = QString::fromUtf8(path);
    QString fileName = content;
    fileName = fileName.replace("\n", "");
    return fileName.trimmed();
}

QString ConfigFSIsoManager::getSelectedISO()
{
    const QString content = getSelectedISOPath();
    QString fileName = content.mid(content.lastIndexOf("/") + 1);
    fileName = fileName.replace("\n", "");
    return fileName.trimmed();
}
