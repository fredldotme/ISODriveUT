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
}

static void scanForDiskImages(const QString &basePath, QVariantList &foundFiles, QSet<QString> &seenPaths)
{
    if (basePath.isEmpty() || !QDir(basePath).exists()) {
        return;
    }

    QDirIterator iterator(basePath, QDir::Files, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();

        const QString filePath = iterator.filePath();
        const QString suffix = QFileInfo(filePath).suffix().toLower();
        if (suffix != QStringLiteral("iso") && suffix != QStringLiteral("img")) {
            continue;
        }
        if (seenPaths.contains(filePath)) {
            continue;
        }

        seenPaths.insert(filePath);

        QVariantMap foundFileInfo;
        foundFileInfo.insert("name", iterator.fileName());
        foundFileInfo.insert("path", filePath);
        foundFiles.push_back(foundFileInfo);
    }
}

void FileManager::refresh()
{
    QVariantList foundFiles;
    QSet<QString> seenPaths;
    scanForDiskImages(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), foundFiles, seenPaths);
    scanForDiskImages(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/iso", foundFiles, seenPaths);
    scanForDiskImages(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/flashdrive", foundFiles, seenPaths);

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
