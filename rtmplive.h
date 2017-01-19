#ifndef RTMPLIVE_H
#define RTMPLIVE_H

#include <QMainWindow>
#include <QString>
#include "RTMPRec.h"
#include "RTMPPushFlv.h"
#include <QTimer>
#include "videocap.h"
#include <QImage>
#include "RTMPPushH264.h"
#include "desktopcap.h"

namespace Ui {
class RtmpLive;
}

class RtmpLive : public QMainWindow
{
    Q_OBJECT

public:
    explicit RtmpLive(QWidget *parent = 0);
    ~RtmpLive();
    friend class RTMPRec;
    friend class RTMPPushFlv;
    friend class PushStream;
private:
    Ui::RtmpLive *ui;
    int runFlag;
    RTMPRec *recoder;
    QString dfileName;
    QString durl;
    QTimer *dshowTimer;
    char dshowInfo[100];

    RTMPPushFlv *filePusher;
    QString sfileName;
    QString sfurl;

    int videoDeviceIndex;//0为桌面
    int deviceNums;
    VideoCap *vCap;
    DesktopCap *dCap;
    int width;
    int height;
    int bufSize;
    unsigned char *vbuf;
    unsigned char *rgbbuf;
//    QImage srcImg;
    QTimer *calTimer;
    int calTime;

    PushStream *videoPusher;
    bool pushFlag;

public slots:
    void selectDownloadPage();
    void selectPushPage();
    void selectPlayPage();

    void doDownStartButton();
    void doDownStopButton();
    void doShowDownInfo(const char*info);
    void doShowDownInfoPerSec();

    void doSendFileStartButton();
    void doSendFileStopButton();
    void doSendFileExit();
    void doShowSendFInfo(const char*info);

    void doSendVideoStartButton();
    void doSendVideoStopButton();
    void doUpdateDevice();
    void doDispVideo();
    void doCalTime();

};

#endif // RTMPLIVE_H
