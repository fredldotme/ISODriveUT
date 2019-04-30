#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

class FileManager : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QVariantList foundFiles READ foundFiles NOTIFY foundFilesChanged)
    FileManager(QObject *parent = nullptr);

public slots:
    void refresh();
    bool removeFile(const QString& filePath);

private:
    QVariantList foundFiles();
    QVariantList m_foundFiles;

signals:
    void foundFilesChanged();

};

#endif // FILEMANAGER_H
