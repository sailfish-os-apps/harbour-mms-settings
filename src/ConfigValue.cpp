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

#include "ConfigValue.h"
#include "ConfigDebug.h"
#include "MmsEngine.h"

#include <MGConfItem>

#include <QFile>
#include <QDomDocument>
#include <QDomElement>
#include <QDomText>

#define SCHEMA_FILE "/usr/share/glib-2.0/schemas/org.nemomobile.mms.sim.gschema.xml"

ConfigValue::ConfigValue(QObject* aParent):
    QObject(aParent), iItem(NULL), iEngine(NULL)
{
}

QString ConfigValue::key() const
{
    return iKey;
}

void ConfigValue::setKey(QString aKey)
{
    QDEBUG(((void*)this) << aKey);
    if (iKey != aKey) {
        QString oldPath = dconfPath();
        iKey = aKey;
        emit keyChanged();
        updateDefaultValue();
        if (dconfPath() != oldPath) updateMGConfItem();
    }
}

QString ConfigValue::imsi() const
{
    return iImsi;
}

void ConfigValue::setImsi(QString aImsi)
{
    QDEBUG(((void*)this) << aImsi);
    if (iImsi != aImsi) {
        QString oldPath = dconfPath();
        iImsi = aImsi;
        emit imsiChanged();
        if (dconfPath() != oldPath) updateMGConfItem();
    }
}

QVariant ConfigValue::value() const
{
    return iValue;
}

void ConfigValue::setValue(QVariant aValue)
{
    QVariant value(aValue);
    if (value.convert(iDefaultValue.type()) && iItem) {
        if (iValue != value) {
            iValue = value;
            iItem->set(value);
            iItem->sync();
        }
    } else {
        QDEBUG(iKey << "ignoring" << aValue);
    }
}

QVariant ConfigValue::defaultValue() const
{
    return iDefaultValue;
}

MmsEngine* ConfigValue::engine() const
{
    return iEngine;
}

void ConfigValue::setEngine(MmsEngine* aEngine)
{
    QDEBUG(((void*)this) << ((void*)aEngine) <<
           (aEngine ? aEngine->version() : QString()));
    if (iEngine != aEngine) {
        QString oldPath = dconfPath();
        if (iEngine) iEngine->disconnect(this);
        iEngine = aEngine;
        if (iEngine) {
            connect(iEngine, SIGNAL(availableChanged()),
                SLOT(onEngineAvailableChanged()));
        }
        emit engineChanged();
        if (dconfPath() != oldPath) updateMGConfItem();
    }
}

QString ConfigValue::dconfPath()
{
    if (iEngine && iEngine->available()) {
        if (!iImsi.isEmpty() && !iKey.isEmpty()) {
            QString path;
            if (iEngine->laterThan(1,0,21)) {
                path.append("/imsi/").append(iImsi).append("/mms/");
            } else {
                path.append("/").append(iImsi).append("/");
            }
            return path.append(iKey);
        }
    }
    return QString();
}

void ConfigValue::onEngineAvailableChanged()
{
    updateMGConfItem();
}

void ConfigValue::onValueChanged()
{
    QDEBUG(iItem->value());
    updateValue(iItem->value());
}

void ConfigValue::updateMGConfItem()
{
    QVariant value;
    if (iItem) {
        delete iItem;
        iItem = NULL;
    }
    if (iDefaultValue.isValid()) {
        QString path = dconfPath();
        if (!path.isEmpty()) {
            iItem = new MGConfItem(path, this);
            connect(iItem, SIGNAL(valueChanged()), SLOT(onValueChanged()));
            value = iItem->value();
            if (!value.isValid()) value = iDefaultValue;
        }
        QDEBUG(dconfPath());
    }
    updateValue(value);
}

void ConfigValue::updateValue(QVariant aValue)
{
    if (aValue.convert(iDefaultValue.type()) && aValue != iValue) {
        QDEBUG(iKey << "=" << aValue);
        iValue = aValue;
        emit valueChanged();
    }
}

static QDomElement keyElement(QDomDocument& aSchema, QString aName)
{
    QDomElement schemalist = aSchema.documentElement();
    if (schemalist.tagName() == "schemalist") {
        QDomElement schema = schemalist.firstChildElement("schema");
        if (!schema.isNull()) {
            QDomNode node = schema.firstChild();
            while (!node.isNull()) {
                QDomElement key = node.toElement();
                if (!key.isNull() && key.tagName() == "key") {
                    if (key.attribute("name") == aName) {
                        return key;
                    }
                }
                node = node.nextSibling();
            }
        }
    }
    return QDomElement();
}

static QVariant keyDefault(QDomElement& aKey)
{
    QVariant value;
    QDomElement defaultValue = aKey.firstChildElement("default");
    QDomText defaultTextNode = defaultValue.firstChild().toText();
    if (!defaultTextNode.isNull()) {
        QString type = aKey.attribute("type");
        QString text = defaultTextNode.data();
        QDEBUG(aKey.attribute("name") << type << text);
        if (type == "s") {
            if (text.startsWith('\'') && text.endsWith('\'')) {
                text = text.mid(1,text.length()-2);
            }
            value = QVariant(text);
        } else if (type == "u") {
            bool ok = false;
            uint n = text.toUInt(&ok);
            if (ok) value = QVariant(n);
        } else if (type == "b") {
            value = QVariant(text == "true");
        } else {
            QDEBUG("Unexpected value type" << type);
        }
    }
    return value;
}

void ConfigValue::updateDefaultValue()
{
    QVariant oldValue = iDefaultValue;
    if (!iKey.isEmpty()) {
        QFile file(SCHEMA_FILE);
        if (file.open(QIODevice::ReadOnly)) {
            QDomDocument doc("schema");
            if (doc.setContent(&file)) {
                QDomElement key = keyElement(doc, iKey);
                if (!key.isNull()) {
                    iDefaultValue = keyDefault(key);
                    QDEBUG(iDefaultValue);
                }
            }
            file.close();
        }
    }

    if (iDefaultValue != oldValue) {
        QDEBUG(iKey << "=" << iDefaultValue);
        emit defaultValueChanged();
    }
}
