#include "filemanager.h"

#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStandardPaths>
#include <unistd.h>

FileManager::FileManager(QObject *parent) : QObject(parent)
{
}

void FileManager::refresh()
{
    QVariantList foundFiles;
    const QString downloadsDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    const QString documentsDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QStringList roots = {
        downloadsDir,
        documentsDir + QStringLiteral("/iso"),
        documentsDir + QStringLiteral("/flashdrive")
    };
    const QStringList filters = { QStringLiteral("*.iso"), QStringLiteral("*.img") };

    for (const QString &root : roots) {
        if (root.isEmpty())
            continue;

        QDirIterator iterator(root, filters, QDir::Files, QDirIterator::Subdirectories);
        while (iterator.hasNext()) {
            iterator.next();

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
