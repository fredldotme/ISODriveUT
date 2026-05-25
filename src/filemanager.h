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
    Q_PROPERTY(QStringList searchPaths READ searchPaths WRITE setSearchPaths NOTIFY searchPathsChanged)
    FileManager(QObject *parent = nullptr);

public slots:
    void refresh();
    bool removeFile(const QString& filePath);
    QStringList searchPaths() const;
    void setSearchPaths(const QStringList &searchPaths);

private:
    QVariantList foundFiles();
    QVariantList m_foundFiles;
    QStringList m_searchPaths;

signals:
    void foundFilesChanged();
    void searchPathsChanged();

};

#endif // FILEMANAGER_H
