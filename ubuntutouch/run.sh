#!/bin/sh

export QT_QUICK_CONTROLS_MOBILE=true
export QT_QUICK_CONTROLS_STYLE=Suru

exec $PWD/ISODrive "$@"
