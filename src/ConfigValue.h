/*
  Copyright (C) 2014 Jolla Ltd.
  Contact: Slava Monich <slava.monich@jolla.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CONFIGVALUE_H
#define CONFIGVALUE_H

#include <QObject>
#include <QString>
#include <QVariant>

class MGConfItem;
class MmsEngine;

class ConfigValue: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString key READ key WRITE setKey NOTIFY keyChanged)
    Q_PROPERTY(QString imsi READ imsi WRITE setImsi NOTIFY imsiChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QVariant defaultValue READ defaultValue NOTIFY defaultValueChanged)
    Q_PROPERTY(MmsEngine* engine READ engine WRITE setEngine NOTIFY engineChanged)

public:
    ConfigValue(QObject* aParent = NULL);

    QString key() const;
    void setKey(QString aKey);

    QString imsi() const;
    void setImsi(QString aImsi);

    QVariant value() const;
    void setValue(QVariant aValue);

    QVariant defaultValue() const;

    MmsEngine* engine() const;
    void setEngine(MmsEngine* aEngine);

public slots:
    void onEngineAvailableChanged();
    void onValueChanged();

signals:
    void keyChanged();
    void imsiChanged();
    void valueChanged();
    void defaultValueChanged();
    void engineChanged();

private:
    void updateMGConfItem();
    void updateDefaultValue();
    QString dconfPath();

private:
    MGConfItem* iItem;
    MmsEngine* iEngine;
    QString iKey;
    QString iImsi;
    QVariant iDefaultValue;
};

#endif // CONFIGVALUE_H
