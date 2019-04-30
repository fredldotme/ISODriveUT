#ifndef UTISOMANAGER_H
#define UTISOMANAGER_H

#include "genericisomanager.h"
#include "commandrunner.h"

const QString SYSFS_ENABLE =
        QStringLiteral("/sys/devices/virtual/android_usb/android0/enable");
const QString SYSFS_FEATURES =
        QStringLiteral("/sys/devices/virtual/android_usb/android0/functions");
const QString SYSFS_IMG_FILE =
        QStringLiteral("/sys/devices/virtual/android_usb/android0/f_mass_storage/lun/file");

class UtIsoManager : public GenericIsoManager
{
    Q_OBJECT
    Q_PROPERTY(QString userPassword READ userPassword WRITE setUserPassword NOTIFY userPasswordChanged)
    Q_PROPERTY(QString selectedISO READ getSelectedISO NOTIFY selectedISOChanged)

public:
    explicit UtIsoManager(QObject *parent = nullptr);

    Q_INVOKABLE void enableISO(QString fileName) Q_DECL_OVERRIDE;
    Q_INVOKABLE bool isEnabledISO(QString fileName) Q_DECL_OVERRIDE;
    Q_INVOKABLE void resetISO() Q_DECL_OVERRIDE;
    Q_INVOKABLE bool validatePassword();

private:
    QString userPassword();
    void setUserPassword(const QString& password);
    bool enabled();
    void setEnabled(bool enabled);
    QString getSelectedISO();
    QString getSelectedISOPath();

    QString m_userPassword;
    CommandRunner* m_commandRunner = nullptr;

signals:
    void selectedISOChanged();
    void selectionFailed();
    void userPasswordChanged();

};

#endif // UTISOMANAGER_H
