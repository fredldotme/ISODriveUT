#ifndef COMMON_H
#define COMMON_H

#include <QString>

// DBus interface to the Canonical property service
const QString PROPERTY_SERVICE_PATH =
        QStringLiteral("/com/canonical/PropertyService");
const QString PROPERTY_SERVICE_OBJ =
        QStringLiteral("com.canonical.PropertyService");

// Constants for android0 interface support
const QString ANDROID0_SYSFS_ENABLE =
        QStringLiteral("/sys/devices/virtual/android_usb/android0/enable");
const QString ANDROID0_SYSFS_FEATURES =
        QStringLiteral("/sys/devices/virtual/android_usb/android0/functions");
const QString ANDROID0_SYSFS_IMG_FILE =
        QStringLiteral("/sys/devices/virtual/android_usb/android0/f_mass_storage/lun/file");

#endif // COMMON_H
