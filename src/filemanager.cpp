#include "filemanager.h"

#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QSet>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <unistd.h>

FileManager::FileManager(QObject *parent) : QObject(parent)
{
    this->m_searchPaths = QStringList() << QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
}

void FileManager::refresh()
{
    QVariantList foundFiles;

    const QStringList paths = this->m_searchPaths.isEmpty()
            ? QStringList() << QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)
            : this->m_searchPaths;
    QSet<QString> seen;

    for (const QString &rootPath : paths) {
        QDirIterator iterator(rootPath,
            QStringList() << "*.iso" << "*.img", QDir::Files, QDirIterator::Subdirectories);

        while (iterator.hasNext()) {
            iterator.next();
            const QString filePath = iterator.filePath();
            if (seen.contains(filePath))
                continue;

            seen.insert(filePath);
            qDebug() << filePath << "matches";

            QVariantMap foundFileInfo;
            foundFileInfo.insert("name", iterator.fileName());
            foundFileInfo.insert("path", filePath);
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

QStringList FileManager::searchPaths() const
{
    return this->m_searchPaths;
}

void FileManager::setSearchPaths(const QStringList &searchPaths)
{
    if (this->m_searchPaths == searchPaths)
        return;

    this->m_searchPaths = searchPaths;
    emit searchPathsChanged();
}
