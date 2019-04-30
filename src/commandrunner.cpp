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

void CommandRunner::providePassword(const QString& password)
{
    this->m_process->write(password.toUtf8());
    this->m_process->write("\n");
}
