QT       += core gui concurrent network
#TEMPLATE = lib
DESTDIR = ../SSLIBS
#CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

#QMAKE_CXXFLAGS += -Wno-unused-parameter
#QMAKE_CXXFLAGS += -Wno-unused-variable
#DEFINES += QT_NO_WARNING_OUTPUT\
#                        QT_NO_DEBUG_OUTPUT

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


#non app
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    newdownloader.cpp

HEADERS += \
    mainwindow.h \
    newdownloader.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

###

SSCoreDIR = ../..

#windows{
#DEFINES += \
#M_PI = 3.141592653589793\
#M_2PI = 6.283185307179586\
#M_PI_2 = 1.570796326794897
#}


INCLUDEPATH += \
$$SSCoreDIR/SSCode \
$$SSCoreDIR/SSCode/VSOP2013 \
$$SSCoreDIR/SSTest\
$$SSCoreDIR/SSTest/Android/sstest/src/main/jni\
$$SSCoreDIR/SSTest/Android/sstest/src/main/cpp

android{
    QT += androidextras
    assets.path = /assets/SSData
    assets.files = SSData/*
    INSTALLS+=assets
    ANDROID_ABIS = armeabi-v7a
    ANDROID_EXTRA_LIBS += $$files( $$DESTDIR/*.so , true )
    LIBS += -L$$DESTDIR -lSSLib_armeabi-v7a
}


RESOURCES += \
    BasicStyle.qrc

ANDROID_ABIS = armeabi-v7a
