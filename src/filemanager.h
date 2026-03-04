#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QStringList>

class FileManager : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QVariantList foundFiles READ foundFiles NOTIFY foundFilesChanged)
    Q_PROPERTY(QStringList customPaths READ customPaths NOTIFY customPathsChanged)
    Q_PROPERTY(bool includeImgFiles READ includeImgFiles NOTIFY includeImgFilesChanged)
    FileManager(QObject *parent = nullptr);

    Q_INVOKABLE void addCustomPath(const QString& path);
    Q_INVOKABLE void removeCustomPath(int index);
    Q_INVOKABLE void setIncludeImgFiles(bool include);

public slots:
    void refresh();
    bool removeFile(const QString& filePath);

private:
    QVariantList foundFiles();
    QVariantList m_foundFiles;
    QStringList m_customPaths;
    bool m_includeImgFiles = false;

    QStringList customPaths() const { return m_customPaths; }
    bool includeImgFiles() const { return m_includeImgFiles; }

signals:
    void foundFilesChanged();
    void customPathsChanged();
    void includeImgFilesChanged();

};

#endif // FILEMANAGER_H
