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

import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.mms.settings.qofono 1.0
import harbour.mms.settings.config 1.0

Page {
    id: page

    // Predefined user agents:
    readonly property var userAgentOptions: [
        {
            text:  "Jolla",
            menu:  "Jolla",
            value: "Mozilla/5.0 (Sailfish; Jolla)"
        },{
            text: "Nokia N9",
            menu: "Nokia N9",
            value: "Mozilla/5.0 (MeeGo; NokiaN9) AppleWebKit/534.13 (KHTML, likeGecko) NokiaBrowser/8.5.0 Mobile Safari/534.13"
        },{
            text:  "iOS",
            menu:  "iOS (iPhone 5)",
            value: "Mozilla/5.0 (iPhone; U; CPU iPhone OS 4_0 like Mac OS X; en-us) AppleWebKit/532.9 (KHTML, like Gecko) Version/4.0.5 Mobile/8A293 Safari/6531.22.7"
        },{
            text:  "Windows",
            menu:  "Windows (Lumia 1020)",
            value: "Mozilla/5.0 (compatible; MSIE 10.0; Windows Phone 8.0; Trident/6.0; IEMobile/10.0; ARM; Touch; NOKIA; Lumia 1020)"
        },{
            text:  "Android",
            menu:  "Android (Galaxy S4)",
            value: "Mozilla/5.0 (Linux; U; Android 4.2; en-us; GT-I9500 Build/JDQ39) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30"
        }
    ]

    // Predefined user agent profiles:
    readonly property var userAgentProfileOptions: [
        {
            text:  "Jolla",
            menu:  "Jolla",
            value: "http://static.jolla.com/uaprof/Jolla.xml"
        },{
            text:  "Nokia N9",
            menu:  "Nokia N9",
            value: "http://nds1.nds.nokia.com/uaprof/NN9-00r100-R.xml"
        },{
            text:  "Windows",
            menu:  "Windows (Lumia)",
            value: "http://nds1.nds.nokia.com/uaprof/Nokia800r100.xml"
        },{
            text:  "iPhone",
            menu:  "iPhone",
            value: "http://iphonemms.apple.com/iphone/uaprof-2MB.rdf"
        }
    ]

    // Predefined message sizes:
    readonly property var maxMessageSizeOptions: [
        {
            text:  qsTr("max-message-size-text-small"),
            menu:  qsTr("max-message-size-menu-small-100k"),
            value: 102400
        },{
            text:  qsTr("max-message-size-text-medium"),
            menu:  qsTr("max-message-size-menu-medium-300k"),
            value: 307200
        },{
            text:  qsTr("max-message-size-text-large"),
            menu:  qsTr("max-message-size-menu-large-600k"),
            value: 614400
        },{
            text:  qsTr("max-message-size-text-extra-large"),
            menu:  qsTr("max-message-size-menu-extra-large-1M"),
            value: 1048576
        }
    ]

    // Predefined pixel limits:
    readonly property var maxPixelsOptions: [
        {
            text:  qsTr("max-pixels-text-default"),
            menu:  qsTr("max-pixels-menu-default-3MP"),
            value: 3000000
        }
    ]

    property var engine
    property bool simAvailable: simManager.present && simManager.subscriberIdentity
    property bool startAnimationPlaying: startTimer.running || !engine || !engine.available

    OfonoManager { id: ofonoManager }

    OfonoSimManager {
        id: simManager
        modemPath: ofonoManager.defaultModem
        onSubscriberIdentityChanged: if (subscriberIdentity && engine) engine.migrateSettings(subscriberIdentity)
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: startAnimationPlaying ? splashScreen.height : content.height

        VerticalScrollDecorator { }

        ViewPlaceholder {
            id: placeholder
            enabled: false
            opacity: enabled ? 1.0 : 0.0

            Behavior on opacity { FadeAnimation { } }

            states: [
                State {
                    name: "error"
                    PropertyChanges { target: placeholder; enabled: true }
                },
                State {
                    name: "NoSim"
                    extend: "error"
                    when: !simAvailable

                    PropertyChanges {
                        target: placeholder
                        text: qsTr("no-sim-card-placeholder")
                    }
                }
            ]
        }

        Column {
            id: splashScreen
            visible: startAnimationPlaying && simAvailable
            x: (page.width - childrenRect.width)/2
            y: (page.height - busyIndicator.height)/2 - pleaseWait.height - spacing
            spacing: Theme.paddingLarge
            Text {
                id: pleaseWait
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font {
                    pixelSize: Theme.fontSizeExtraLarge
                    family: Theme.fontFamilyHeading
                }
                color: Theme.highlightColor
                opacity: 0.6
                text: qsTr("please-wait-splash")
            }
            BusyIndicator {
                id: busyIndicator
                anchors.horizontalCenter: parent.horizontalCenter
                size: BusyIndicatorSize.Large
                running: parent.visible
            }
        }

        Column {
            id: content
            visible: !startAnimationPlaying
            enabled: placeholder.state === ""
            opacity: enabled ? 1 : 0
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader {
                title: qsTr("mms-settings-header")
                Image {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: Theme.paddingLarge
                    }
                    source: "image://theme/icon-m-sailfish"
                }
            }

            ValueEditor {
                predefined: userAgentOptions
                imsi: simManager.subscriberIdentity
                engine: page.engine
                key: "user-agent"
                label: qsTr("user-agent-label")
            }

            ValueEditor {
                predefined: userAgentProfileOptions
                imsi: simManager.subscriberIdentity
                engine: page.engine
                key: "user-agent-profile"
                label: qsTr("user-agent-profile-label")
            }

            ValueEditor {
                predefined: maxMessageSizeOptions
                imsi: simManager.subscriberIdentity
                engine: page.engine
                key: "max-message-size"
                label: qsTr("max-message-size-label")
                placeholderText: qsTr("max-message-size-placeholder")
                customTextLabel: qsTr("max-message-size-custom-units")
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
                formatter: QtObject {
                    function format(size) {
                        if ((size % 1048576) === 0) {
                            return qsTr("max-message-size-format-MB").arg(size/1048576)
                        } else if ((size % 1024) === 0) {
                            return qsTr("max-message-size-format-kB").arg(size/1024)
                        } else {
                            return size;
                        }
                    }
                }
            }

            ValueEditor {
                predefined: maxPixelsOptions
                imsi: simManager.subscriberIdentity
                engine: page.engine
                key: "max-pixels"
                label: qsTr("max-pixels-label")
                placeholderText: qsTr("max-pixels-placeholder")
                customTextLabel: qsTr("max-pixels-custom-units")
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
                formatter: QtObject {
                    function format(size) {
                        return qsTr("max-pixels-format").arg(size);
                    }
                }
            }
        }
    }

    IntValidator {
        id: intValidator
        bottom: 0
        top: 2147483647
    }

    Timer {
        id: startTimer
        running: true
        repeat: false
        interval: 1000
    }
}
