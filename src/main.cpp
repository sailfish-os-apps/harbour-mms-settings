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

#include <QtGui>
#include <QtQuick>
#include <sailfishapp.h>

#include "ConfigDebug.h"
#include "ConfigValue.h"
#include "MmsEngine.h"
#include "qofono/qofonomanager.h"
#include "qofono/qofonosimmanager.h"

#define PLUGIN_PREFIX "harbour.mms.settings"

void registerOfonoTypes(const char* uri, int v1 = 1, int v2 = 0)
{
    qmlRegisterType<QOfonoManager>(uri, v1, v2, "OfonoManager");
    qmlRegisterType<QOfonoSimManager>(uri, v1, v2, "OfonoSimManager");
}

void registerConfigTypes(const char* uri, int v1 = 1, int v2 = 0)
{
    qmlRegisterType<ConfigValue>(uri, v1, v2, "ConfigValue");
    qmlRegisterType<MmsEngine>(uri, v1, v2, "MmsEngine");
}

int main(int argc, char *argv[])
{
    int result = 0;
    QGuiApplication* app = SailfishApp::application(argc, argv);

    QLocale locale;
    QTranslator* translator = new QTranslator(app);
    QString transDir = SailfishApp::pathTo("translations").toLocalFile();
    QString transFile("harbour-mms-settings");
    if (translator->load(locale, transFile, "-", transDir) ||
        translator->load(transFile, transDir)) {
        app->installTranslator(translator);
    } else {
        QDEBUG("Failed to load translator for" << locale);
        delete translator;
    }

    registerOfonoTypes(PLUGIN_PREFIX ".qofono");
    registerConfigTypes(PLUGIN_PREFIX ".config");

    QQuickView *view = SailfishApp::createView();
    view->setSource(SailfishApp::pathTo("qml/main.qml"));
    view->show();

    result = app->exec();

    delete view;
    delete app;
    return result;
}
