include(common.pri)

TEMPLATE = app

QT += quick\
core

CONFIG += \
console \
c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
SSCode \
SSCode/VSOP2013 \
SSCore/SSTest\
SSCore/SSTest/Android/sstest/src/main/jni\
SSCore/SSTest/Android/sstest/src/main/cpp


HEADERS += \
    SSCode/VSOP2013/ELPMPP02.hpp \
    SSCode/VSOP2013/VSOP2013.hpp \
    SSCode/SSAngle.hpp \
    SSCode/SSConstellation.hpp \
    SSCode/SSCoordinates.hpp \
    SSCode/SSEvent.hpp \
    SSCode/SSHTM.hpp \
    SSCode/SSIdentifier.hpp \
    SSCode/SSImportGJ.hpp \
    SSCode/SSImportHIP.hpp \
    SSCode/SSImportMPC.hpp \
    SSCode/SSImportNGCIC.hpp \
    SSCode/SSImportSKY2000.hpp \
    SSCode/SSJPLDEphemeris.hpp \
    SSCode/SSMatrix.hpp \
    SSCode/SSMoonEphemeris.hpp \
    SSCode/SSObject.hpp \
    SSCode/SSOrbit.hpp \
    SSCode/SSPSEphemeris.hpp \
    SSCode/SSPlanet.hpp \
    SSCode/SSStar.hpp \
    SSCode/SSTLE.hpp \
    SSCode/SSTime.hpp \
    SSCode/SSUtilities.hpp \
    SSCode/SSVPEphemeris.hpp \
    SSCode/SSVector.hpp \
    SSCode/SSView.hpp \
    SSTest/Android/sstest/src/main/jni/JNIUtilities.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSAngle.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSCoordinates.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSDate.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSDegMinSec.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSEvent.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSEventTime.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSHourMinSec.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSIdentifier.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSJPLDEphemeris.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSMatrix.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSObject.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSObjectArray.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSPass.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSRTS.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSSpherical.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSTime.h \
    SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSVector.h \
    SSTest/SSTest.h \
 \#    mainwindow.h
    mainwindow.h

SOURCES += \
        SSCode/SSAngle.cpp \
        SSCode/SSConstellation.cpp \
        SSCode/SSCoordinates.cpp \
        SSCode/SSEvent.cpp \
        SSCode/SSHTM.cpp \
        SSCode/SSIdentifier.cpp \
        SSCode/SSImportGJ.cpp \
        SSCode/SSImportHIP.cpp \
        SSCode/SSImportMPC.cpp \
        SSCode/SSImportNGCIC.cpp \
        SSCode/SSImportSKY2000.cpp \
        SSCode/SSJPLDEphemeris.cpp \
        SSCode/SSMatrix.cpp \
        SSCode/SSMoonEphemeris.cpp \
        SSCode/SSObject.cpp \
        SSCode/SSOrbit.cpp \
        SSCode/SSPSEphemeris.cpp \
        SSCode/SSPlanet.cpp \
        SSCode/SSStar.cpp \
        SSCode/SSTLE.cpp \
        SSCode/SSTime.cpp \
        SSCode/SSUtilities.cpp \
        SSCode/SSVPEphemeris.cpp \
        SSCode/SSVector.cpp \
        SSCode/SSView.cpp \
        SSCode/VSOP2013/ELPMPP02.cpp \
        SSCode/VSOP2013/VSOP2013.cpp \
        SSCode/VSOP2013/VSOP2013p1.cpp \
        SSCode/VSOP2013/VSOP2013p2.cpp \
        SSCode/VSOP2013/VSOP2013p3.cpp \
        SSCode/VSOP2013/VSOP2013p4.cpp \
        SSCode/VSOP2013/VSOP2013p5.cpp \
        SSCode/VSOP2013/VSOP2013p6.cpp \
        SSCode/VSOP2013/VSOP2013p7.cpp \
        SSCode/VSOP2013/VSOP2013p8.cpp \
        SSCode/VSOP2013/VSOP2013p9.cpp \
        mainwindow.cpp

android {
        SOURCES += \
        SSTest/Android/sstest/src/main/cpp/native-lib.cpp \
        SSTest/Android/sstest/src/main/jni/JNIUtilities.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSAngle.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSCoordinates.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSDate.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSDegMinSec.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSEvent.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSEventTime.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSHourMinSec.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSIdentifier.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSJPLDEphemeris.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSMatrix.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSObject.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSObjectArray.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSPass.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSRTS.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSSpherical.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSTime.cpp \
        SSTest/Android/sstest/src/main/jni/com_southernstars_sscore_JSSVector.cpp \
        main.cpp
}

windows {
        SOURCES += \
#        SSTest/SSTest.cpp
        main.cpp
}
RESOURCES += qml.qrc


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

ANDROID_ABIS = armeabi-v7a

#FORMS += \
#    mainwindow.ui

FORMS += \
    mainwindow.ui



