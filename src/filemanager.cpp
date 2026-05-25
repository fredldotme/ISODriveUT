#include "filemanager.h"

#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSet>
#include <unistd.h>

FileManager::FileManager(QObject *parent) : QObject(parent)
{
}

void FileManager::refresh()
{
    QVariantList foundFiles;
    const QStringList roots = {
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QStringLiteral("/iso"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QStringLiteral("/flashdrive")
    };

    QSet<QString> seen;
    for (const QString &root : roots) {
        if (root.isEmpty() || !QDir(root).exists())
            continue;

        QDirIterator iterator(root, QDir::Files, QDirIterator::Subdirectories);
        while (iterator.hasNext()) {
            iterator.next();

            const QFileInfo info(iterator.filePath());
            const QString suffix = info.suffix().toLower();
            if (suffix != QStringLiteral("iso") && suffix != QStringLiteral("img"))
                continue;
            if (seen.contains(info.canonicalFilePath()))
                continue;
            seen.insert(info.canonicalFilePath());

            qDebug() << iterator.filePath() << "matches";
            QVariantMap foundFileInfo;
            foundFileInfo.insert("name", iterator.fileName());
            foundFileInfo.insert("path", iterator.filePath());
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
