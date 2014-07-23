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

#include "MmsEngine.h"
#include "ConfigDebug.h"

#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

MmsEngine::MmsEngine(QObject* aParent):
    QObject(aParent),
    iMajor(0),
    iMinor(0),
    iMicro(0),
    iAvailable(false),
    iVersionKnown(false)
{
    // Request migration for the current SIM
    call(SLOT(onInitialMigrateFinished(QDBusPendingCallWatcher*)),
        "migrateSettings", QString());
}

QDBusPendingCall MmsEngine::call(QString aMethod, QVariant aArg)
{
    return QDBusInterface("org.nemomobile.MmsEngine", "/",
        "org.nemomobile.MmsEngine", QDBusConnection::systemBus()).
        asyncCall(aMethod, aArg);
}

void MmsEngine::call(const char* aSlot, QString aMethod, QVariant aArg)
{
    connect(new QDBusPendingCallWatcher(call(aMethod, aArg), this),
        SIGNAL(finished(QDBusPendingCallWatcher*)), aSlot);
}

void MmsEngine::migrateSettings(QString aImsi)
{
    QDEBUG(aImsi);
    call(QString("migrateSettings"), aImsi);
}

void MmsEngine::onInitialMigrateFinished(QDBusPendingCallWatcher* aCall)
{
    QASSERT(!iAvailable);
    call(SLOT(onGetVersionFinished(QDBusPendingCallWatcher*)), "getVersion");
    aCall->deleteLater();
}

void MmsEngine::onGetVersionFinished(QDBusPendingCallWatcher* aCall)
{
    QDBusPendingReply<int,int,int,QString> reply = *aCall;
    if (reply.isError()) {
        QDEBUG(reply.error().message());
        QDEBUG("Assuming 1.0.20");
        iMajor = 1;
        iMinor = 0;
        iMicro = 20;
    } else {
        iMajor = reply.argumentAt<0>();
        iMinor = reply.argumentAt<1>();
        iMicro = reply.argumentAt<2>();
        QDEBUG(version());
        if (iMajor) {
            iVersionKnown = true;
            emit versionKnownChanged();
        } else {
            QDEBUG("Assuming 1.0.22");
            iMajor = 1;
            iMinor = 0;
            iMicro = 22;
        }
    }
    QASSERT(!iAvailable);
    iAvailable = true;
    emit versionChanged();
    emit availableChanged();
    aCall->deleteLater();
}

bool MmsEngine::available() const
{
    return iAvailable;
}

bool MmsEngine::versionKnown() const
{
    return iVersionKnown;
}

bool MmsEngine::laterThan(int aMajor, int aMinor, int aMicro) const
{
    if (iMajor > aMajor) {
        return true;
    } else if (iMajor == aMajor) {
        if (iMinor > aMinor) {
            return true;
        } else if (iMinor == aMinor) {
            return (iMicro > aMicro);
        }
    }
    return false;
}

QString MmsEngine::version() const
{
    if (iMajor || iMinor || iMicro) {
        return QString().sprintf("%d.%d.%d",iMajor,iMinor,iMicro);
    } else {
        return QString();
    }
}
