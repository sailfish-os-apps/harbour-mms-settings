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

#include "qofonosettings.h"
#include "qofonomodem.h"

#include "dbus/ofonosettings.h"

class QOfonoSettingsPrivate : public QOfonoModem
{
public:
    QOfonoSettingsPrivate(QObject* parent);
    ~QOfonoSettingsPrivate();
    QString group;
    OfonoSettings* oSettings;
    QHash<QString,QString> settings;
};

QOfonoSettingsPrivate::QOfonoSettingsPrivate(QObject* parent) :
    QOfonoModem(parent), oSettings(NULL)
{
}

QOfonoSettingsPrivate::~QOfonoSettingsPrivate()
{
    if (oSettings) delete oSettings;
}

QOfonoSettings::QOfonoSettings(QObject* parent) :
    QObject(parent)
{
    d_ptr = new QOfonoSettingsPrivate(this);
    connect(d_ptr, SIGNAL(interfacesChanged(QStringList)),
        this, SLOT(onInterfacesChanged(QStringList)));
    connect(d_ptr, SIGNAL(modemPathChanged(QString)),
        this, SLOT(onModemPathChanged(QString)));
}

QOfonoSettings::~QOfonoSettings()
{
    delete d_ptr;
}

QString QOfonoSettings::modemPath() const
{
    return d_ptr->modemPath();
}

void QOfonoSettings::setModemPath(QString path)
{
    d_ptr->setModemPath(path);
}

QString QOfonoSettings::group() const
{
    return d_ptr->group;
}

void QOfonoSettings::setGroup(QString group)
{
    if (d_ptr->group != group) {
        d_ptr->group = group;
        emit groupChanged(group);
        if (group.isEmpty()) {
            deleteOfonoProxy();
        } else {
            createOfonoProxy();
        }
    }
}

bool QOfonoSettings::available() const
{
    return d_ptr->oSettings != NULL;
}

QString QOfonoSettings::get(QString key) const
{
    return d_ptr->settings.value(key);
}

void QOfonoSettings::set(QString key, QString value)
{
    if (d_ptr->oSettings && !key.isEmpty() && get(key) != value) {
        if (value.isEmpty()) {
            d_ptr->settings.remove(key);
        } else {
            d_ptr->settings.insert(key, value);
        }
        emit changed(key, value);
        d_ptr->oSettings->Set(d_ptr->group, key, value);
    }
}

void QOfonoSettings::reset(QString key)
{
    if (d_ptr->oSettings && !key.isEmpty()) {
        d_ptr->oSettings->Reset(d_ptr->group, key);
    }
}

void QOfonoSettings::resetGroup()
{
    if (d_ptr->oSettings) {
        d_ptr->oSettings->ResetGroup(d_ptr->group);
    }
}

void QOfonoSettings::createOfonoProxy(bool signalAvailabilityChange)
{
    bool changed = false;
    if (!d_ptr->oSettings) {
        d_ptr->oSettings = new OfonoSettings("org.ofono", d_ptr->modemPath(),
            QDBusConnection::systemBus(), d_ptr);
        if (d_ptr->oSettings->isValid()) {
            changed = true;
            connect(d_ptr->oSettings, SIGNAL(Changed(QString,QString,QString)),
                this, SLOT(onChanged(QString,QString,QString)));
        } else {
            d_ptr->oSettings = NULL;
        }
    }
    // createOfonoProxy() is always invoked when something has changed
    // so significantly that update is absolutely required
    updateSettings();
    if (changed && signalAvailabilityChange) availabilityChanged(true);
}

void QOfonoSettings::deleteOfonoProxy()
{
    if (d_ptr->oSettings) {
        delete d_ptr->oSettings;
        d_ptr->oSettings = NULL;
        availabilityChanged(false);
        updateSettings();
    }
}

void QOfonoSettings::updateSettings()
{
    if (d_ptr->oSettings) {
        QDBusPendingReply<OfonoArrayOfSettings> reply =
            d_ptr->oSettings->GetGroup(d_ptr->group);
        reply.waitForFinished();
        update(reply.value().get());
    } else {
        update(QHash<QString,QString>());
    }
}

void QOfonoSettings::update(QHash<QString,QString> settings)
{
    QHash<QString,QString> oldSettings = d_ptr->settings;
    QStringList removedKeys = oldSettings.keys();
    QStringList keys = settings.keys();
    d_ptr->settings = settings;
    foreach (QString key, keys) {
        QString value = settings.value(key);
        removedKeys.removeOne(key);
        if (oldSettings.value(key) != value) {
            emit changed(key, value);
        }
    }
    foreach (QString key, removedKeys) {
        emit changed(key, QString());
    }
}

void QOfonoSettings::onModemPathChanged(QString path)
{
    emit modemPathChanged(path);
    const bool wasAvailable = available();
    if (d_ptr->oSettings) {
        delete d_ptr->oSettings;
        d_ptr->oSettings = NULL;
    }
    createOfonoProxy(false);
    const bool isAvailable = available();
    if (wasAvailable != isAvailable) {
        availabilityChanged(isAvailable);
    }
}

void QOfonoSettings::onInterfacesChanged(QStringList list)
{
    if (list.contains(OfonoSettings::staticInterfaceName())) {
        if (!d_ptr->oSettings) {
            createOfonoProxy();
        }
    } else {
        deleteOfonoProxy();
    }
}

void QOfonoSettings::onChanged(QString group, QString key, QString value)
{
    if (d_ptr->group == group) {
        QString oldValue = d_ptr->settings.value(key);
        if (oldValue != value) {
            if (value.isEmpty()) {
                d_ptr->settings.remove(key);
            } else {
                d_ptr->settings.insert(key, value);
            }
            emit changed(key, value);
        }
    }
}
