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

#ifndef QOFONOSettings_H
#define QOFONOSettings_H

#include <QObject>
#include <QString>

#include "qofono_global.h"

class QOfonoSettingsPrivate;

class QOFONOSHARED_EXPORT QOfonoSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool available READ available NOTIFY availabilityChanged)
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)

public:
    explicit QOfonoSettings(QObject* parent = NULL);
    ~QOfonoSettings();

    QString modemPath() const;
    void setModemPath(QString path);

    QString group() const;
    void setGroup(QString path);

    bool available() const;

    Q_INVOKABLE QString get(QString key) const;
    Q_INVOKABLE void set(QString key, QString value);
    Q_INVOKABLE void reset(QString key);
    Q_INVOKABLE void resetGroup();

Q_SIGNALS:
    void changed(QString key, QString value);
    void modemPathChanged(QString modemPath);
    void groupChanged(QString group);
    void availabilityChanged(bool available);

private slots:
    void onChanged(QString group, QString key, QString value);
    void onModemPathChanged(QString path);
    void onInterfacesChanged(QStringList list);

private:
    void update(QHash<QString,QString> settings);
    void createOfonoProxy(bool signalAvailabilityChange = true);
    void deleteOfonoProxy();
    void updateSettings();

private:
    QOfonoSettingsPrivate* d_ptr;
};

#endif // QOFONOSettings_H
