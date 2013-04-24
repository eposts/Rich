#!/bin/bash

set -e
export LANG=zh_CN.utf8
WINEDEBUG=-all env WINEPREFIX=/opt/wine/  /opt/wine/wine/bin/wine /opt/wine/drive_c/Program\ Files/Tencent/QQ/Bin/QQ.exe >/dev/null 2>&1 &
