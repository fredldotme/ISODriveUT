#ifndef COMMON_H
#define COMMON_H

#include <QString>

// DBus interface to the Canonical property service
const QString PROPERTY_SERVICE_PATH =
        QStringLiteral("/com/canonical/PropertyService");
const QString PROPERTY_SERVICE_OBJ =
        QStringLiteral("com.canonical.PropertyService");

#endif // COMMON_H
