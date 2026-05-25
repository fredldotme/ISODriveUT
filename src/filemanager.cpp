#include "filemanager.h"

#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <unistd.h>

namespace {
QStringList imageSearchRoots()
{
    QStringList roots;
    roots << QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

    const QString documentsRoot =
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (!documentsRoot.isEmpty()) {
        roots << documentsRoot + QStringLiteral("/iso");
        roots << documentsRoot + QStringLiteral("/flashdrive");
    }

    return roots;
}
}

FileManager::FileManager(QObject *parent) : QObject(parent)
{
}

void FileManager::refresh()
{
    QVariantList foundFiles;
    const QStringList nameFilters = QStringList() << "*.iso" << "*.img";

    for (const QString &root : imageSearchRoots()) {
        if (!QDir(root).exists())
            continue;

        QDirIterator iterator(root, nameFilters, QDir::Files,
                              QDirIterator::Subdirectories);

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
