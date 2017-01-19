#-------------------------------------------------
#
# Project created by QtCreator 2017-01-14T14:25:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RtmpLive
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        rtmplive.cpp \
    RTMPRec.cpp \
    sockInit.cpp \
    RTMPPushFlv.cpp \
    videoInput.cpp \
    videocap.cpp \
    librtmp_send264.cpp \
    RtmpH264.cpp \
    RTMPPushH264.cpp \
    desktopcap.cpp \
    vlcplayer.cpp \
    videoplayer.cpp

HEADERS  += rtmplive.h \
    x264.h \
    RTMPRec.h \
    sockInit.h \
    RTMPPushFlv.h \
    videoInput.h \
    videocap.h \
    librtmp_send264.h \
    RtmpH264.h \
    RTMPPushH264.h \
    desktopcap.h \
    vlcplayer.h \
    videoplayer.h



FORMS    += rtmplive.ui \
    videoplayer.ui


LIBS+=-LD:\Android\QtWorks\RtmpLive\libs\ -llibx264 \
        -llibrtmp  -lavutil -lswscale -llibvlc -llibvlccore

RESOURCES += \
    images.qrc
