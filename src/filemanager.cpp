#include "filemanager.h"

#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <unistd.h>

FileManager::FileManager(QObject *parent) : QObject(parent)
{
}

void FileManager::refresh()
{
    QVariantList foundFiles;
    QDirIterator iterator(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), 
        QStringList() << "*.iso", QDir::Files, QDirIterator::Subdirectories);

    while (iterator.hasNext()) {
        iterator.next();

        qDebug() << iterator.filePath() << "matches";
        QVariantMap foundFileInfo;

        foundFileInfo.insert("name", iterator.fileName());
        foundFileInfo.insert("path", iterator.filePath());

        qDebug() << foundFileInfo;
        foundFiles.push_back(foundFileInfo);
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
