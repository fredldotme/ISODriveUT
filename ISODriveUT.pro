TARGET = ISODrive

QT += quick dbus

INCLUDEPATH += \
    $$PWD/ubuntutouch/src \
    $$PWD/src

DEPENDPATH += \
    $$PWD/ubuntutouch/src \
    $$PWD/src

# figure out the current build architecture
CLICK_ARCH=$$system(dpkg-architecture -qDEB_HOST_ARCH)

# do not remove this line, it is required by the IDE even if you do
# not substitute variables in the manifest file
UBUNTU_MANIFEST_FILE = $$PWD/ubuntutouch/manifest.json.in

# substitute the architecture in the manifest file
manifest_file.output  = manifest.json
manifest_file.CONFIG += no_link \
    add_inputs_as_makefile_deps \
    target_predeps

manifest_file.commands = sed s/@CLICK_ARCH@/$$CLICK_ARCH/g ${QMAKE_FILE_NAME} > ${QMAKE_FILE_OUT}
manifest_file.input = UBUNTU_MANIFEST_FILE
QMAKE_EXTRA_COMPILERS += manifest_file

# installation path of the manifest file
mfile.CONFIG += no_check_exist
mfile.files  += $$OUT_PWD/manifest.json
mfile.path = /

# AppArmor profile
apparmor.files += $$PWD/ubuntutouch/isodrive.apparmor
apparmor.path = /

# Desktop launcher
desktop.files += $$PWD/ubuntutouch/isodrive.desktop
desktop.path = /

# Desktop launcher icon
desktopicon.files += $$PWD/ubuntutouch/isodrive.png
desktopicon.path = /

# Run script
runscript.files += $$PWD/ubuntutouch/run.sh
runscript.path = /

target.path = /
INSTALLS += mfile apparmor desktop desktopicon runscript target


SOURCES += \
    $$PWD/src/ISODrive.cpp \
    $$PWD/src/filemanager.cpp \
    $$PWD/src/commandrunner.cpp \
    $$PWD/src/utisomanager.cpp

HEADERS += \
    $$PWD/src/filemanager.h \
    $$PWD/src/genericisomanager.h \
    $$PWD/src/commandrunner.h \
    $$PWD/src/utisomanager.h

RESOURCES += \
    qml-ut.qrc
