#include "filemanager.h"

#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QSet>
#include <unistd.h>

FileManager::FileManager(QObject *parent) : QObject(parent)
{
}

void FileManager::refresh()
{
    QVariantList foundFiles;
    QSet<QString> seenPaths;

    const QStringList roots = searchRoots();
    for (const QString& root : roots) {
        const QDir rootDir(root);
        if (!rootDir.exists()) {
            qDebug() << "Skipping missing directory:" << root;
            continue;
        }

        QDirIterator iterator(root,
                              QDir::Files,
                              QDirIterator::Subdirectories);

        while (iterator.hasNext()) {
            iterator.next();
            if (!isBootImageFile(iterator.fileName())) {
                continue;
            }

            const QString absolutePath = iterator.filePath();
            if (seenPaths.contains(absolutePath)) {
                continue;
            }
            seenPaths.insert(absolutePath);

            qDebug() << absolutePath << "matches";
            QVariantMap foundFileInfo;
            foundFileInfo.insert("name", iterator.fileName());
            foundFileInfo.insert("path", absolutePath);
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

QStringList FileManager::searchRoots() const
{
    const QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    const QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QStringList roots;
    roots << downloadsPath;

    if (!documentsPath.isEmpty()) {
        roots << QDir(documentsPath).filePath("iso");
        roots << QDir(documentsPath).filePath("flashdrive");
    }

    return roots;
}

bool FileManager::isBootImageFile(const QString &fileName)
{
    const QString lowerName = fileName.toLower();
    return lowerName.endsWith(".iso") || lowerName.endsWith(".img");
}
