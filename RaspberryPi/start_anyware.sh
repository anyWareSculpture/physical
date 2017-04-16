#!/bin/sh
echo "Starting anyWare" >> $HOME/anyware_log.txt
chromium-browser --app-id=`cat $HOME/app.txt` --start-fullscreen --start-maximized --enable-logging=stderr --v=0 --log-level=0 >> $HOME/anyware_log.txt 2>&1
