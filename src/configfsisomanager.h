#ifndef CONFIGFSISOMANAGER_H
#define CONFIGFSISOMANAGER_H

#include <QObject>
#include <QDBusInterface>
#include "genericisomanager.h"
#include "commandrunner.h"

class ConfigFSIsoManager : public GenericIsoManager
{
    Q_OBJECT
    Q_PROPERTY(QString userPassword READ userPassword WRITE setUserPassword NOTIFY userPasswordChanged)
    Q_PROPERTY(QString selectedISO READ getSelectedISO NOTIFY selectedISOChanged)
    Q_PROPERTY(bool usbActive READ isUsbActive NOTIFY usbActiveChanged)

public:
    explicit ConfigFSIsoManager(QObject *parent = nullptr);

    Q_INVOKABLE void enableISO(const QString& fileName, const bool enableSharing = false) Q_DECL_OVERRIDE;
    Q_INVOKABLE bool isEnabledISO(QString fileName) Q_DECL_OVERRIDE;
    Q_INVOKABLE void resetISO() Q_DECL_OVERRIDE;
    Q_INVOKABLE bool validatePassword();
    Q_INVOKABLE bool isUsbActive();
    Q_INVOKABLE void setUsbActive(bool active);

private:
    QString userPassword();
    void setUserPassword(const QString& password);
    QString getSelectedISO();
    QString getSelectedISOPath();

    QString getGadgetRoot();
    QString getConfigRoot();
    void resetUDC();
    void setUDC();

    QString m_userPassword;
    CommandRunner* m_commandRunner = nullptr;

signals:
    void selectedISOChanged();
    void selectionFailed();
    void userPasswordChanged();
    void usbActiveChanged();

};

#endif // CONFIGFSISOMANAGER_H
