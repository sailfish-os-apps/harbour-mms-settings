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

#ifndef MMSENGINE_H
#define MMSENGINE_H

#include <QObject>
#include <QDBusPendingCall>

class QDBusPendingCallWatcher;

class MmsEngine: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)

public:
    MmsEngine(QObject* aParent = NULL);

    QString version() const;
    bool available() const;
    bool laterThan(int aMajor, int aMinor, int aMicro) const;

    Q_INVOKABLE void migrateSettings(QString aImsi);

private:
    QDBusPendingCall call(QString aMethod, QVariant aArg = QVariant());
    void call(const char* aSlot, QString aMethod, QVariant aArg = QVariant());

signals:
    void versionChanged();
    void availableChanged();

public slots:
    void onInitialMigrateFinished(QDBusPendingCallWatcher* aCall);
    void onGetVersionFinished(QDBusPendingCallWatcher* aCall);

private:
    int iMajor;
    int iMinor;
    int iMicro;
    bool iAvailable;
};

#endif // MMSENGINE_H
