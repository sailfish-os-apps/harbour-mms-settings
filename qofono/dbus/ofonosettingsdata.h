/****************************************************************************
**
** Copyright (C) 2014 Jolla Ltd.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#ifndef QOFONOSettingsData_H
#define QOFONOSettingsData_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtDBus/QtDBus>

class OfonoArrayOfSettings: public QObject
{
    Q_OBJECT
public:
    explicit OfonoArrayOfSettings(QObject* parent = NULL);
    OfonoArrayOfSettings(const OfonoArrayOfSettings& other);
    OfonoArrayOfSettings& operator=(const OfonoArrayOfSettings& other);

    QHash<QString,QString> get() const { return settings; }
    void demarshall(const QDBusArgument& arg);

private:
    QHash<QString,QString> settings;
};

class OfonoGroupedSettings: public QObject
{
    Q_OBJECT
public:
    explicit OfonoGroupedSettings(QObject* parent = NULL);
    OfonoGroupedSettings(const OfonoGroupedSettings& other);
    OfonoGroupedSettings& operator=(const OfonoGroupedSettings& other);

    QHash<QString,OfonoArrayOfSettings> get() const { return settings; }
    void demarshall(const QDBusArgument& arg);

private:
    QHash<QString,OfonoArrayOfSettings> settings;
};

const QDBusArgument& operator>>(const QDBusArgument& arg, OfonoArrayOfSettings& obj);
const QDBusArgument& operator>>(const QDBusArgument& arg, OfonoGroupedSettings& obj);

Q_DECLARE_METATYPE(OfonoArrayOfSettings);
Q_DECLARE_METATYPE(OfonoGroupedSettings);

#endif // QOFONOSettingsData_H
