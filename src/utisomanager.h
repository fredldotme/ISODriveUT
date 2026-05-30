#ifndef UTISOMANAGER_H
#define UTISOMANAGER_H

#include "genericisomanager.h"
#include "commandrunner.h"

#include <QDBusInterface>

class UtIsoManager : public GenericIsoManager
{
    Q_OBJECT
    Q_PROPERTY(QString userPassword READ userPassword WRITE setUserPassword NOTIFY userPasswordChanged)
    Q_PROPERTY(QString selectedISO READ getSelectedISO NOTIFY selectedISOChanged)
    Q_PROPERTY(bool usbActive READ isUsbActive NOTIFY usbActiveChanged)

public:
    explicit UtIsoManager(QObject *parent = nullptr);

    Q_INVOKABLE void enableISO(const QString& fileName, const bool enableSharing = false) Q_DECL_OVERRIDE;
    Q_INVOKABLE bool isEnabledISO(QString fileName) Q_DECL_OVERRIDE;
    Q_INVOKABLE void resetISO() Q_DECL_OVERRIDE;
    Q_INVOKABLE bool validatePassword();
    Q_INVOKABLE bool isUsbActive();
    Q_INVOKABLE void setUsbActive(bool active);

private:
    QString userPassword();
    void setUserPassword(const QString& password);
    bool enabled();
    void setEnabled(bool enabled);
    QString getSelectedISO();
    QString getSelectedISOPath();

    QString m_userPassword;
    CommandRunner* m_commandRunner = nullptr;
    QDBusInterface* m_propertyService = nullptr;


signals:
    void selectedISOChanged();
    void selectionFailed();
    void userPasswordChanged();
    void usbActiveChanged();

};

#endif // UTISOMANAGER_H
