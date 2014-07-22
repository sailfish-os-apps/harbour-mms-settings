TARGET = harbour-mms-settings

QT += dbus xml
CONFIG += link_pkgconfig
CONFIG += sailfishapp
PKGCONFIG += mlite5
QMAKE_CXXFLAGS += -Wno-unused-parameter

SOURCES += \
    src/main.cpp \
    src/ConfigValue.cpp \
    src/MmsEngine.cpp

HEADERS += \
    src/MmsEngine.h \
    src/ConfigDebug.h \
    src/ConfigValue.h

INCLUDEPATH += \
    qofono

OTHER_FILES += \
    qml/main.qml \
    qml/cover/*.qml \
    qml/pages/*.qml \
    qml/cover/*.png \
    rpm/harbour-mms-settings.changes \
    rpm/harbour-mms-settings.spec \
    translations/*.ts \
    harbour-mms-settings.desktop

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
