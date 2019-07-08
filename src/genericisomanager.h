#ifndef GENERICISOMANAGER_H
#define GENERICISOMANAGER_H

#include <QObject>

class GenericIsoManager : public QObject {
    Q_OBJECT

public:
    GenericIsoManager(QObject* parent = nullptr) : QObject(parent) {}

    virtual Q_INVOKABLE void enableISO(const QString& fileName, const bool enableSharing = false)
    {
        Q_UNUSED(fileName)
        Q_UNUSED(enableSharing)
    }

    virtual Q_INVOKABLE bool isEnabledISO(QString fileName)
    {
        Q_UNUSED(fileName)
        return false;
    }

    virtual Q_INVOKABLE void resetISO() {}

};

#endif // GENERICISOMANAGER_H
