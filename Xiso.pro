QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    Detector.h \
    mainwindow.h

FORMS += \
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
LIBS += -L"C:/SLDevice/SDK/lib/x64/Release" -L"C:/SLDevice/SDK/lib/PleoraLibs/x64"
LIBS += -lSLDeviceLib -lSLImage -llibtiff -llegacy_stdio_definitions

include('../Qylon/Qylon.pri')

RESOURCES += \
    Resources.qrc
