#include "filemanager.h"

#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <unistd.h>

FileManager::FileManager(QObject *parent) : QObject(parent)
{
    // Default to Downloads folder
    m_customPaths.append(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
}

void FileManager::addCustomPath(const QString& path)
{
    if (!m_customPaths.contains(path)) {
        m_customPaths.append(path);
        emit customPathsChanged();
        refresh();
    }
}

void FileManager::removeCustomPath(int index)
{
    if (index >= 0 && index < m_customPaths.size()) {
        // Don't remove the last remaining path (default Downloads)
        if (m_customPaths.size() > 1) {
            m_customPaths.removeAt(index);
            emit customPathsChanged();
            refresh();
        }
    }
}

void FileManager::setIncludeImgFiles(bool include)
{
    if (m_includeImgFiles != include) {
        m_includeImgFiles = include;
        emit includeImgFilesChanged();
        refresh();
    }
}

void FileManager::refresh()
{
    QVariantList foundFiles;
    QStringList extensions;
    extensions << "*.iso";
    if (m_includeImgFiles) {
        extensions << "*.img";
    }

    // Scan all configured paths
    for (const QString& searchPath : m_customPaths) {
        QDirIterator iterator(searchPath, extensions, QDir::Files, QDirIterator::Subdirectories);

        while (iterator.hasNext()) {
            iterator.next();

            qDebug() << iterator.filePath() << "matches";
            QVariantMap foundFileInfo;

            foundFileInfo.insert("name", iterator.fileName());
            foundFileInfo.insert("path", iterator.filePath());
            // Add file type info for UI
            QString suffix = QFileInfo(iterator.fileName()).suffix().toLower();
            foundFileInfo.insert("type", suffix);

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
