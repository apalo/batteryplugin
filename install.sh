#!/bin/bash

PLUGIN_NAME=battery_plugin
PLUGIN_BIN_DIR=/usr/lib/xfce4/panel/plugins/
PLUGIN_DESK_DIR=/usr/share/xfce4/panel/plugins/

if [ $EUID -ne 0 ]; then
    echo "Must be root to install the plugin. Do this at your own risk."
    echo "Example su -c ./install.sh"
    exit 1
fi

if [ ! -f lib${PLUGIN_NAME}.so ]; then
    echo "Plugin .so file does not exist"
    exit 1
fi

if [ ! -f ${PLUGIN_NAME}.desktop ]; then
    echo "Plugin .desktop file does not exist"
    exit 1
fi

echo "Installing plugin to $PLUGIN_BIN_DIR"
cp lib${PLUGIN_NAME}.so ${PLUGIN_BIN_DIR}

echo "Installing desktop file to $PLUGIN_DESK_DIR"
cp ${PLUGIN_NAME}.desktop ${PLUGIN_DESK_DIR}
