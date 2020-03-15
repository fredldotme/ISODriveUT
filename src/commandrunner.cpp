#include "commandrunner.h"

#include <QDebug>

CommandRunner::CommandRunner(QObject *parent) :
    QObject(parent),
    m_process(new QProcess(this))
{
    connect(this->m_process, &QProcess::stateChanged, this, [=](QProcess::ProcessState state) {
        if (state == QProcess::NotRunning) {
            qDebug() << "Command stopped";
        }
    });
    connect(this->m_process, &QProcess::readyReadStandardError,
            this, [=]() {
        const QByteArray stdErrContent = this->m_process->readAllStandardError();
        qDebug() << stdErrContent;
        if (stdErrContent.contains("userpasswd")) {
            emit passwordRequested();
        }
    });
}

void CommandRunner::sudo(const QStringList &command)
{
    QStringList cmd = QStringList{"-S", "-p", "userpasswd"} + command;
    qDebug() << "running" << cmd.join(" ");

    this->m_process->start("sudo", cmd, QProcess::ReadWrite);
}

QByteArray CommandRunner::readFile(const QString &absolutePath)
{
    sudo(QStringList{"cat" , absolutePath});
    this->m_process->waitForFinished();
    const QByteArray value = this->m_process->readAllStandardOutput();
    qDebug() << absolutePath << "=" << value;
    return value;
}

bool CommandRunner::writeFile(const QString &absolutePath, const QByteArray &value)
{
    const QStringList writeCommand {
        QStringLiteral("/bin/sh"), QStringLiteral("-c"),
        QStringLiteral("echo '%1' | tee %2").arg(value, absolutePath)
    };
    sudo(writeCommand);
    this->m_process->waitForFinished();
    return (this->m_process->exitCode() == 0);
}

bool CommandRunner::mkDir(const QString &absolutePath)
{
    const QStringList writeCommand {
        QStringLiteral("/bin/sh"), QStringLiteral("-c"),
        QStringLiteral("/bin/mkdir '%1'").arg(absolutePath)
    };
    sudo(writeCommand);
    this->m_process->waitForFinished();
    return (this->m_process->exitCode() == 0);
}

bool CommandRunner::ln(const QString& source, const QString& newTarget)
{
    const QStringList writeCommand {
        QStringLiteral("/bin/sh"), QStringLiteral("-c"),
        QStringLiteral("/bin/ln -s '%1' '%2'").arg(source, newTarget)
    };
    sudo(writeCommand);
    this->m_process->waitForFinished();
    return (this->m_process->exitCode() == 0);
}

QString CommandRunner::getConfigFsMountPoint()
{
    const QStringList writeCommand {
        QStringLiteral("/bin/sh"), QStringLiteral("-c"),
        QStringLiteral("/bin/mount | /bin/grep configfs")
    };

    sudo(writeCommand);
    this->m_process->waitForFinished();

    const QString value = QString(this->m_process->readAllStandardOutput()).trimmed();
    QStringList values = value.split(' ', QString::SkipEmptyParts);

    if (values.size() < 5)
        return QStringLiteral("");
    return values[2];
}

QString CommandRunner::getUDCController()
{
    const QStringList writeCommand {
        QStringLiteral("/bin/sh"), QStringLiteral("-c"),
        QStringLiteral("/usr/bin/getprop sys.usb.controller")
    };

    sudo(writeCommand);
    this->m_process->waitForFinished();

    const QString value = QString(this->m_process->readAllStandardOutput()).trimmed();
    return value;
}

void CommandRunner::providePassword(const QString& password)
{
    this->m_process->write(password.toUtf8());
    this->m_process->write("\n");
}

bool CommandRunner::validatePassword()
{
    const QStringList idCommand {
        QStringLiteral("id"), QStringLiteral("-u")
    };
    sudo(idCommand);
    this->m_process->waitForFinished();
    const QByteArray output = this->m_process->readAllStandardOutput();
    return (output.trimmed() == "0");
}
