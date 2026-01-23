#include "filemanager.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStandardPaths>
#include <QStringList>
#include <unistd.h>

FileManager::FileManager(QObject *parent) : QObject(parent)
{
}

void FileManager::refresh()
{
    QVariantList foundFiles;
    const QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    const QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QStringList searchPaths = QStringList()
        << downloadsPath
        << QDir(documentsPath).filePath("iso")
        << QDir(documentsPath).filePath("flashdrive");
    const QStringList nameFilters = QStringList() << "*.iso" << "*.img";

    for (const QString &path : searchPaths) {
        QDir dir(path);
        if (!dir.exists()) {
            continue;
        }

        QDirIterator iterator(dir.absolutePath(), nameFilters, QDir::Files, QDirIterator::Subdirectories);

        while (iterator.hasNext()) {
            iterator.next();

            qDebug() << iterator.filePath() << "matches";
            QVariantMap foundFileInfo;

            foundFileInfo.insert("name", iterator.fileName());
            foundFileInfo.insert("path", iterator.filePath());

            qDebug() << foundFileInfo;
            foundFiles.push_back(foundFileInfo);
        }
    }

    this->m_foundFiles = foundFiles;
    emit foundFilesChanged();
}

QVariantList FileManager::foundFiles()
{
    return this->m_foundFiles;
}

bool FileManager::removeFile(const QString &filePath)
{
    const QString absolutePath = filePath.startsWith("file://") ? filePath.mid(7) : filePath;
    const bool ret = QFile::remove(absolutePath);
    sync();
    return ret;
}
