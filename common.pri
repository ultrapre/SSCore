
SSCoreDIR = .


QT += \
        core \
        gui \
        widgets \
        concurrent \
        script \
        multimedia \
        multimediawidgets \
        positioning \
        serialport \
        printsupport \
        network \
        opengl \
        sensors



android {
        QT += androidextras
#        ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
        ANDROID_PACKAGE            = com.ss.app
        ANDROID_MINIMUM_VERSION    = 21
        ANDROID_TARGET_VERSION     = 21
        ANDROID_APP_NAME           = SSAPP

        assets.path    = /assets/
        assets.files   = SSData
        INSTALLS += assets

        LIBS += -landroid # need by
}

#android {

#        DEFINES += \
#                __ANDROID_API__=$$replace(ANDROID_PLATFORM,"android-","")
#}

# Not on android
windows{
DEFINES += \
M_PI = 3.141592653589793\
M_2PI = 6.283185307179586\
M_PI_2 = 1.570796326794897
}

#silas
android{
#QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable
}


