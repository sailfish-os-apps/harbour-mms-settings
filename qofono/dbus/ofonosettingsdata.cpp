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

#include "ofonosettingsdata.h"

OfonoArrayOfSettings::OfonoArrayOfSettings(QObject* parent) :
    QObject(parent)
{
}

OfonoArrayOfSettings::OfonoArrayOfSettings(const OfonoArrayOfSettings& other) :
    QObject(other.parent()),
    settings(other.settings)
{
}

OfonoArrayOfSettings& OfonoArrayOfSettings::operator=(const OfonoArrayOfSettings& other)
{
    settings = other.settings;
    return *this;
}

void OfonoArrayOfSettings::demarshall(const QDBusArgument& arg)
{
    settings.clear();
    arg.beginMap();
    while (!arg.atEnd()) {
        QString key, value;
        arg.beginMapEntry();
        arg >> key >> value;
        arg.endMapEntry();
        settings.insert(key, value);
    }
    arg.endMap();
}

const QDBusArgument& operator>>(const QDBusArgument& arg, OfonoArrayOfSettings& obj)
{
    obj.demarshall(arg);
    return arg;
}

OfonoGroupedSettings::OfonoGroupedSettings(QObject* parent) :
    QObject(parent)
{
}

OfonoGroupedSettings::OfonoGroupedSettings(const OfonoGroupedSettings& other) :
    QObject(other.parent()),
    settings(other.settings)
{
}

OfonoGroupedSettings& OfonoGroupedSettings::operator=(const OfonoGroupedSettings& other)
{
    settings = other.settings;
    return *this;
}

void OfonoGroupedSettings::demarshall(const QDBusArgument& arg)
{
    settings.clear();
    arg.beginArray();
    while (!arg.atEnd()) {
        QString group;
        OfonoArrayOfSettings values;
        arg.beginStructure();
        arg >> group >> values;
        arg.endStructure();
        settings.insert(group, values);
    }
    arg.endArray();
}

const QDBusArgument& operator>>(const QDBusArgument& arg, OfonoGroupedSettings& obj)
{
    obj.demarshall(arg);
    return arg;
}
