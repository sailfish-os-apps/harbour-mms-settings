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
import harbour.mms.settings.gsettings 1.0

Page {
    id: page

    // Predefined user agents:
    readonly property var userAgentOptions: [
        "Jolla","Mozilla/5.0 (Sailfish; Jolla)",
        "Nokia N9","Mozilla/5.0 (MeeGo; NokiaN9) AppleWebKit/534.13 (KHTML, likeGecko) NokiaBrowser/8.5.0 Mobile Safari/534.13",
        "iOS (iPhone 5)","Mozilla/5.0 (iPhone; U; CPU iPhone OS 4_0 like Mac OS X; en-us) AppleWebKit/532.9 (KHTML, like Gecko) Version/4.0.5 Mobile/8A293 Safari/6531.22.7",
        "Windows (Lumia 1020)","Mozilla/5.0 (compatible; MSIE 10.0; Windows Phone 8.0; Trident/6.0; IEMobile/10.0; ARM; Touch; NOKIA; Lumia 1020)",
        "Android (Galaxy S4)","Mozilla/5.0 (Linux; U; Android 4.2; en-us; GT-I9500 Build/JDQ39) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30"
    ]

    // Predefined user agent profiles:
    readonly property var userAgentProfileOptions: [
        "Jolla","http://static.jolla.com/uaprof/Jolla.xml",
        "Nokia N9","http://nds1.nds.nokia.com/uaprof/NN9-00r100-R.xml",
        "Windows (Lumia)","http://nds1.nds.nokia.com/uaprof/Nokia800r100.xml"
    ]

    // Predefined message sizes:
    readonly property var maxMessageSizeOptions: [
        qsTr("Small"),"102400",
        qsTr("Medium"),"307200",
        qsTr("Large"),"614400",
        qsTr("Extra large"),"1048576"
    ]

    // Predefined pixel limits:
    readonly property var maxPixelsOptions: [
        qsTr("Default"),"3000000"
    ]

    property string ofonoModem: ofonoManager.modems[0]
    property bool simAvailable: simManager.present && simManager.subscriberIdentity

    OfonoManager { id: ofonoManager }

    OfonoSimManager {
        id: simManager
        modemPath: ofonoModem
    }

    GSettings {
        id: settings
        schema.path: simAvailable ? ("/" + simManager.subscriberIdentity + "/") : ""
        schema.id: "org.nemomobile.mms.sim"
        onChanged: {
            if (key == userAgentProfileEditor.name) {
                userAgentProfileEditor.value = value
            } else if (key == userAgentEditor.name) {
                userAgentEditor.value = value
            } else if (key == maxMessageSizeEditor.name) {
                maxMessageSizeEditor.value = value
            } else if (key == maxPixelsEditor.name) {
                maxPixelsEditor.value = value
            }
        }
    }

    Component.onCompleted: {
        userAgentEditor.value = settings.userAgent
        userAgentProfileEditor.value = settings.userAgentProfile
        maxMessageSizeEditor.value = settings.maxMessageSize
        maxPixelsEditor.value = settings.maxPixels

        // Settings validators at construction doesn't do the right thing
        // (value gets marked as invalid even though it's perfectly valid)
        maxMessageSizeEditor.validator = intValidator
        maxPixelsEditor.validator = intValidator
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

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
                        text: qsTr("MMS settings are not available without SIM card")
                    }
                }
            ]
        }

        Column {
            id: content
            enabled: placeholder.state === ""
            opacity: enabled ? 1 : 0
            width: parent.width
            spacing: Theme.paddingLarge

            PageHeader { title: qsTr("MMS Settings") }

            ValueEditor {
                id: userAgentEditor
                predefined: userAgentOptions
                name: "userAgent"
                label: qsTr("User-Agent:")
                onValueChanged: settings.userAgent = value
            }

            ValueEditor {
                id: userAgentProfileEditor
                predefined: userAgentProfileOptions
                name: "userAgentProfile"
                label: qsTr("User Agent profile:")
                onValueChanged: settings.userAgentProfile = value
            }

            ValueEditor {
                id: maxMessageSizeEditor
                predefined: maxMessageSizeOptions
                name: "maxMessageSize"
                label: qsTr("Maximum message size:")
                placeholderText: qsTr("Maximum size (bytes)")
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
                formatter: QtObject {
                    function format(size) {
                        if ((size % 1048576) === 0) {
                            return qsTr("%1 MB").arg(size/1048576)
                        } else if ((size % 1024) === 0) {
                            return qsTr("%1 kB").arg(size/1024)
                        } else {
                            return size;
                        }
                    }
                }
                onValueChanged: settings.maxMessageSize = value
            }

            ValueEditor {
                id: maxPixelsEditor
                predefined: maxPixelsOptions
                name: "maxPixels"
                label: qsTr("Maximum image size:")
                placeholderText: qsTr("Maximum size (pixels)")
                inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhDigitsOnly
                formatter: QtObject {
                    function format(size) {
                        return qsTr("%1 pixels").arg(size);
                    }
                }
                onValueChanged: settings.maxPixels = value
            }
        }
    }

    IntValidator {
        id: intValidator
        bottom: 0
        top: 2147483647
    }
}
