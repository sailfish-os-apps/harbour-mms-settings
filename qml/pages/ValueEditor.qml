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
import harbour.mms.settings.config 1.0

Column {
    id: root
    width: parent.width

    property var predefined: []
    property string customMenuText
    property string customPlaceholder
    property alias imsi: configValue.imsi
    property alias key: configValue.key
    property alias engine: configValue.engine
    property alias label: comboBox.label
    property alias validator: customText.validator
    property alias placeholderText: customText.placeholderText
    property alias inputMethodHints: customText.inputMethodHints
    property var formatter
    visible: configValue.defaultValue !== undefined

    // Internal properties
    property bool updatingValue: false
    property bool updatingIndex: false
    property bool updatingText: false

    onPredefinedChanged: resetMenu()

    Component.onCompleted: updateSelection()

    ConfigValue {
        id: configValue
        onValueChanged: if (!updatingValue) updateSelection()
    }

    ComboBox {
        id: comboBox
        width: parent.width
        menu: ContextMenu { }
        onCurrentIndexChanged: {
            var wasUpdatingValue = updatingValue
            updatingValue = true
            if (currentIndex === predefined.length/2) {
                configValue.value = customText.text
                if (!updatingIndex) customText.focus = true
            } else {
                configValue.value = predefined[2*currentIndex + 1]
            }
            updatingValue = wasUpdatingValue
        }
    }

    MouseArea {
        id: mouseArea
        property bool menuOpen: menu.parent === mouseArea
        property bool canCopy: customText.validator ? false : true
        visible: comboBox.currentIndex < predefined.length/2
        height: menuOpen ? (menu.height + label.height) : label.height
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: comboBox.labelMargin
            rightMargin: comboBox.labelMargin
        }
        Label {
            id: label
            anchors {
                left: parent.left
                right: parent.right
            }
            text: (comboBox.currentIndex < predefined.length/2) ? format(predefined[comboBox.currentIndex*2+1]) : ""
            color: (parent.canCopy && (parent.pressed || parent.menuOpen)) ? Theme.secondaryHighlightColor : Theme.secondaryColor
            width: parent.width
            font.pixelSize: Theme.fontSizeExtraSmall
            wrapMode: Text.Wrap
        }
        ContextMenu {
            id: menu
            MenuItem {
                text: qsTr("Copy")
                onClicked: Clipboard.text = label.text
            }
        }
        onPressAndHold: if (canCopy) menu.show(mouseArea)
    }

    TextField {
        id: customText
        width: parent.width
        color: Theme.secondaryColor
        visible: comboBox.currentIndex === predefined.length/2
        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
        EnterKey.onClicked: focus = false
        EnterKey.iconSource: "image://theme/icon-m-enter-close"
        onTextChanged: {
            if (!updatingText) {
                var wasUpdatingValue = updatingValue
                updatingValue = true
                configValue.value = text
                updatingValue = wasUpdatingValue
            }
        }
    }

    Component {
        id: menuItemComponent
        MenuItem {}
    }

    function format(text) {
        return formatter ? formatter.format(text) : text
    }

    function resetMenu() {
        var i, n = predefined ? predefined.length/2 : 0
        var menu = comboBox.menu
        for (i = menu.children.length; i > 0 ; i--) {
            menu.children[i-1].destroy()
        }
        if (n > 0) {
            var foundValue = false
            updatingIndex = true
            for (i = 0; i < n; i++) {
                var name = predefined[2*i]
                var predefinedValue = predefined[2*i+1]
                menuItemComponent.createObject(menu._contentColumn, {"text": name } )
                if (predefinedValue == configValue.value) {
                    comboBox.currentIndex = i
                    foundValue = true
                }
            }
            var custom = customMenuText ? customMenuText : qsTr("Custom")
            menuItemComponent.createObject(menu._contentColumn, {"text": custom } )
            if (!foundValue) comboBox.currentIndex = n
            updatingIndex = false
        }
    }

    function updateSelection() {
        var n = predefined ? predefined.length/2 : 0
        if (n > 0) {
            for (var i = 0; i < n; i++) {
                if (configValue.value == predefined[2*i+1]) {
                    updatingIndex = true
                    comboBox.currentIndex = i
                    updatingIndex = false
                    return
                }
            }

            updatingText = true
            customText.text = configValue.value ? configValue.value : ""
            updatingText = false

            updatingIndex = true
            comboBox.currentIndex = n
            updatingIndex = false
        }
    }
}
