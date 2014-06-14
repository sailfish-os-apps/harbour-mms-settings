TARGET = harbour-mms-settings

QT += dbus
CONFIG += link_pkgconfig
CONFIG += sailfishapp
PKGCONFIG += glib-2.0 gio-2.0
QMAKE_CXXFLAGS += -Wno-unused-parameter

SOURCES += \
    src/main.cpp

INCLUDEPATH += \
    gsettings-qt \
    gsettings-qt/src \
    qofono

OTHER_FILES += \
    qml/main.qml \
    qml/cover/*.qml \
    qml/pages/*.qml \
    qml/cover/*.png \
    rpm/harbour-mms-settings.changes.in \
    rpm/harbour-mms-settings.spec \
    translations/*.ts \
    harbour-mms-settings.desktop

# gsettings-qt
SOURCES += \
    gsettings-qt/src/qconftypes.cpp \
    gsettings-qt/src/qgsettings.cpp \
    gsettings-qt/src/util.cpp

HEADERS += \
    gsettings-qt/src/qconftypes.h \
    gsettings-qt/src/qgsettings.h \
    gsettings-qt/src/util.h

SOURCES += \
    gsettings-qt/GSettings/gsettings-qml.cpp \
    gsettings-qt/GSettings/plugin.cpp

HEADERS += \
    gsettings-qt/GSettings/gsettings-qml.h \
    gsettings-qt/GSettings/plugin.h

# qofono
SOURCES += \
    qofono/qofonomanager.cpp \
    qofono/qofonomodem.cpp \
    qofono/qofonosimmanager.cpp \
    qofono/dbus/ofonomanager.cpp \
    qofono/dbus/ofonomodem.cpp \
    qofono/dbus/ofonosimmanager.cpp

HEADERS += \
    qofono/qofonomanager.h \
    qofono/qofonomodem.h \
    qofono/qofonosimmanager.h \
    qofono/dbus/ofonomanager.h \
    qofono/dbus/ofonomodem.h \
    qofono/dbus/ofonosimmanager.h

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n
TRANSLATIONS += \
    translations/harbour-mms-settings.ts \
    translations/harbour-mms-settings-ru.ts
