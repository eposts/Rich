#!/bin/bash

set -e
export LANG=zh_CN.utf8
WINEDEBUG=-all env WINEPREFIX=/opt/wine/  /opt/wine/wine/bin/wine /opt/wine/drive_c/Program\ Files/Tencent/QQDownload/QQDownload.exe >/dev/null 2>&1 &
