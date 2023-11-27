QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BrightContrastControl.cpp \
    Detector.cpp \
    DetectorDialog.cpp \
    Grabber.cpp \
    GrabberDialog.cpp \
    main.cpp \
    mainwindow.cpp \


HEADERS += \
    BrightContrastControl.h \
    Detector.h \
    DetectorDialog.h \
    Grabber.h \
    GrabberDialog.h \
    mainwindow.h \
    GraphicsScene.h \
    GraphicsView.h \
    GraphicsWidget.h

FORMS += \
    BrightContrastControl.ui \
    DetectorDialog.ui \
    GrabberDialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# C:\Program Files\Basler\FramegrabberSDK\lib\visualc
#INCLUDEPATH += "C:/Program Files/Basler/FramegrabberSDK/include"
#LIBS += -L"C:/Program Files/Basler/FramegrabberSDK/lib/visualc"
#LIBS += -lfglib5 -ldisplay_lib

#INCLUDEPATH += $(SL_INCLUDE)
#C:\SLDevice\SDK\lib\x64\Release
#C:\SLDevice\SDK\lib\PleoraLibs\x64
INCLUDEPATH += "C:/SLDevice/SDK/headers" "C:/SLDevice/SDK/headers/GIGEheaders" "C:/SLDevice/SDK/headers/Pleoraheaders"
LIBS += -L"C:/SLDevice/SDK/lib/x64/Release" -L"C:\SLDevice\SDK\lib\x64\Debug" -L"C:/SLDevice/SDK/lib/PleoraLibs/x64"
LIBS += -lSLDeviceLib -lSLDeviceLib_d -lSLImage -llibtiff -llegacy_stdio_definitions -lPvDevice64 -llibpng -lSLDefectCorrection -lSLGIGEInterface -SLHelperLibrary

INCLUDEPATH += "$$(BASLER_FG_SDK_DIR)/include"
DEFINES += GRABBER_ENABLED

contains(QMAKE_TARGET.arch, x86_64) {
    LIBS += -L"$$(BASLER_FG_SDK_DIR)/lib" -L"$$(BASLER_FG_SDK_DIR)/lib/visualc"
    LIBS += -lclsersis -lsiso_genicam -lsiso_log -lXmlParser_MD_VC141_v3_1_Basler_pylon -lfglib5 -lsiso_hal -llibtiff -liolibrt -ldisplay_lib

#clsersis.lib
#iolibrt.lib
#siso_log.lib
#SLDeviceLib_d.lib
#SLImage.lib
#legacy_stdio_definitions.lib
#PvDevice64.lib
} else {
    LIBS += -L"$$(PYLON_DEV_DIR)/lib/win32"
    LIBS += -L"$$(BASLER_FG_SDK_DIR)/lib"
}

#include('../Qylon/Qylon.pri')

DESTDIR = '../Build/'

RESOURCES += \
    Resources.qrc

