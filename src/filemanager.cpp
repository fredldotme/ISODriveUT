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
    QDirIterator iterator(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));

    while (iterator.hasNext()) {
        const QString filePath = iterator.next();
        const QFileInfo fileInfo(filePath);
        const QString loweredFileName = filePath.toLower();

        if (!loweredFileName.endsWith(".iso"))
            continue;

        qDebug() << filePath << "matches";
        QVariantMap foundFileInfo;

        foundFileInfo.insert("name", fileInfo.fileName());
        foundFileInfo.insert("path", fileInfo.absoluteFilePath());

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
    const QString absolutePath = filePath.mid(7);
    const bool ret = QFile::remove(absolutePath);
    sync();
    return ret;
}
