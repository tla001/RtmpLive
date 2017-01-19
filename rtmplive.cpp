#include "rtmplive.h"
#include "ui_rtmplive.h"
#include <QIcon>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextCursor>
#include "sockInit.h"
#include <QDebug>
#include "videoplayer.h"

RtmpLive::RtmpLive(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RtmpLive)
{
    ui->setupUi(this);
    setWindowTitle( QString::fromLocal8Bit("RTMP流媒体演示系统"));
    setWindowIcon(QIcon(":/icos/images/stream.ico"));
    runFlag=0;
    InitSockets();
    /********************保存文件*************************/
    dfileName="";
    durl="";
    recoder=new RTMPRec();
    dshowTimer=new QTimer(this);
    connect(dshowTimer,SIGNAL(timeout()),this,SLOT(doShowDownInfoPerSec()));

    connect(ui->downloadradioButton,SIGNAL(pressed()),this,SLOT(selectDownloadPage()));
    connect(ui->pushradioButton,SIGNAL(pressed()),this,SLOT(selectPushPage()));
    connect(ui->playradioButton,SIGNAL(pressed()),this,SLOT(selectPlayPage()));

    connect(ui->dstartButton1,SIGNAL(clicked(bool)),this,SLOT(doDownStartButton()));
    connect(ui->dstopButton1,SIGNAL(clicked(bool)),this,SLOT(doDownStopButton()));

    ui->downloadradioButton->setChecked(true);
    selectDownloadPage();

    ui->durlEdit->setText("rtmp://live.hkstv.hk.lxdns.com/live/hks");
    ui->dfileEdit->setText("C:/Users/Administrator/Desktop/rec.flv");

    /********************推送文件*************************/
    sfileName="";
    sfurl="";
    filePusher=new RTMPPushFlv();
    connect(ui->sfstartButton,SIGNAL(clicked(bool)),this,SLOT(doSendFileStartButton()));
    connect(ui->sfstopButton,SIGNAL(clicked(bool)),this,SLOT(doSendFileStopButton()));
    connect(filePusher,SIGNAL(nornalExit()),this,SLOT(doSendFileExit()));
    ui->surlEdit->setText("rtmp://live.tla001.cn/live/test1");
    ui->sfileEdit->setText("C:/Users/Administrator/Desktop/rec.flv");


     /********************推送视频*************************/
    width=640;
    height=480;
    rgbbuf=new unsigned char[width*height*3];
    vbuf=NULL;
    connect(ui->svstartButton,SIGNAL(clicked(bool)),this,SLOT(doSendVideoStartButton()));
    connect(ui->svstopButton,SIGNAL(clicked(bool)),this,SLOT(doSendVideoStopButton()));
    connect(ui->videocomboBox,SIGNAL(activated(int)),this,SLOT(doUpdateDevice()));

    vCap=new VideoCap;
    connect(vCap,SIGNAL(capFinish()),this,SLOT(doDispVideo()));
    ui->videocomboBox->insertItem(0,QString::fromLocal8Bit("桌面"));
    doUpdateDevice();
    calTime=0;
    calTimer=new QTimer;
    connect(calTimer,SIGNAL(timeout()),this,SLOT(doCalTime()));

    dCap=new DesktopCap;
    connect(dCap,SIGNAL(capFinish()),this,SLOT(doDispVideo()));

    videoPusher=new PushStream(width,height,AV_PIX_FMT_RGB24);
    pushFlag=false;
}

RtmpLive::~RtmpLive()
{
    delete calTimer;
    delete[] rgbbuf;
    CleanupSockets();
    delete ui;
}
void RtmpLive::selectDownloadPage(){
    if(runFlag>0){
        QMessageBox::warning(this,"Warning",QString::fromLocal8Bit("请停止当前正在运行的程序！"));
    }else if(runFlag==0){
        ui->stackedWidget->setCurrentIndex(0);
        ui->dstopButton1->setEnabled(false);
    }
}
void RtmpLive::selectPushPage(){
    if(runFlag>0){
        QMessageBox::warning(this,"Warning",QString::fromLocal8Bit("请停止当前正在运行的程序！"));
    }else if(runFlag==0){
        ui->stackedWidget->setCurrentIndex(1);
    }
}
void RtmpLive::selectPlayPage(){
    if(runFlag>0){
        QMessageBox::warning(this,"Warning",QString::fromLocal8Bit("请停止当前正在运行的程序！"));
    }else if(runFlag==0){
        videoPlayer player;
        this->hide();
        player.exec();
        this->show();
    }
}

void RtmpLive::doDownStartButton(){
    durl=ui->durlEdit->text();
    dfileName=ui->dfileEdit->text();
    if(durl.isEmpty()){
        QMessageBox::warning(this,"url","rtmp url is empty");
        ui->durlEdit->setFocus();
    }else if(dfileName.isEmpty()){
        QMessageBox::warning(this,"file name","file name is empty");
        dfileName=QFileDialog::getSaveFileName(this,"Save File",QDir::currentPath(),"Videos (*.flv)");
        if(dfileName.isEmpty())
        {
            QMessageBox::information(this,"Error Message",
                                     "Please Select A File");
            return;
        }else{
              ui->dfileEdit->setText(dfileName);
        }
    }
    if(!durl.isEmpty()&&!dfileName.isEmpty()){
        ui->dstartButton1->setEnabled(false);
        recoder->init(durl.toUtf8().data(),dfileName.toUtf8().data());
        recoder->doSave();
        runFlag=1;
        dshowTimer->start(1000);
        ui->dstopButton1->setEnabled(true);
    }
}
void RtmpLive::doDownStopButton(){
    ui->dstopButton1->setEnabled(false);
    recoder->doStop();
    dfileName="";
//    ui->dfileEdit->clear();
//    ui->dfileEdit->setFocus();
    dshowTimer->stop();
    runFlag=0;
    ui->dstartButton1->setEnabled(true);
}
void RtmpLive::doShowDownInfo(const char* info){
    ui->dinfoBrowser->append(QString::fromUtf8(info));
    ui->dinfoBrowser->moveCursor(QTextCursor::End);
}
void RtmpLive::doShowDownInfoPerSec(){
    doShowDownInfo(dshowInfo);
}

void RtmpLive::doSendFileStartButton(){
    sfurl=ui->surlEdit->text();
    sfileName=ui->sfileEdit->text();
    if(sfurl.isEmpty()){
        QMessageBox::warning(this,"url","rtmp url is empty");
        ui->surlEdit->setFocus();
    }else if(sfileName.isEmpty()){
        QMessageBox::warning(this,"file name","file name is empty");
        sfileName=QFileDialog::getOpenFileName(this,"Open File",QDir::currentPath(),"Videos (*.flv)");
        if(sfileName.isEmpty())
        {
            QMessageBox::information(this,"Error Message",
                                     "Please Select A File");
            return;
        }else{
              ui->sfileEdit->setText(sfileName);
        }
    }
    if(!sfurl.isEmpty()&&!sfileName.isEmpty()){
        ui->sfstartButton->setEnabled(false);
        filePusher->init(sfurl.toUtf8().data(), sfileName.toUtf8().data());
        filePusher->doPush();
        runFlag=2;
        ui->sfstopButton->setEnabled(true);
    }
}

void RtmpLive::doSendFileStopButton(){
    ui->sfstopButton->setEnabled(false);
    filePusher->doStop();
    sfileName="";
//    ui->sfileEdit->clear();
//    ui->sfileEdit->setFocus();
    runFlag=0;
    ui->sfstartButton->setEnabled(true);
}
void RtmpLive::doSendFileExit(){
    QMessageBox::information(this,"Finished","push file over");
     ui->sfstopButton->setEnabled(false);
     sfileName="";
//     ui->sfileEdit->clear();
//     ui->sfileEdit->setFocus();
 //    dshowTimer->stop();
     runFlag=0;
     ui->sfstartButton->setEnabled(true);
}
void RtmpLive::doShowSendFInfo(const char*info){
    ui->sfinfoBrowser->append(QString::fromUtf8(info));
    ui->sfinfoBrowser->moveCursor(QTextCursor::End);
}


void RtmpLive::doSendVideoStartButton(){
    if(ui->livecheckBox->checkState())
        pushFlag=true;
    else
        pushFlag=false;
//    qDebug()<<pushFlag;
    ui->svstartButton->setEnabled(false);
    ui->svstopButton->setEnabled(true);
    ui->videocomboBox->setEnabled(false);
    ui->livecheckBox->setEnabled(false);
    videoDeviceIndex=ui->videocomboBox->currentIndex();
    calTime=0;
    calTimer->start(1000);
    if(videoDeviceIndex==0){
        bufSize=dCap->init(width,height);
        vbuf=new unsigned char[bufSize];
        dCap->doCap();
        runFlag=3;
    }else if(videoDeviceIndex>0){
        bufSize=vCap->init(width,height,videoDeviceIndex-1);
        vbuf=new unsigned char[bufSize];
        vCap->doCap();
        runFlag=3;
    }
//    qDebug()<<videoDeviceIndex;
    if(pushFlag){
        sfurl=ui->surlEdit->text();
//        qDebug()<<sfurl;
        if(sfurl.isEmpty()){
            QMessageBox::warning(this,"error","rtmp url invalid");
        }else{
            videoPusher->init(sfurl.toUtf8().data(),25);
            videoPusher->doPush();
        }
    }
}
void RtmpLive::doSendVideoStopButton(){
    if(videoDeviceIndex==0){
        dCap->doStop();
        runFlag=0;
    }else if(videoDeviceIndex>0){
        vCap->doStop();
        runFlag=0;
//        delete[]vbuf;
    }
    if(pushFlag){
        videoPusher->doStop();
    }
    calTimer->stop();
    ui->svstartButton->setEnabled(true);
    ui->svstopButton->setEnabled(false);
    ui->videocomboBox->setEnabled(true);
    ui->livecheckBox->setEnabled(true);
//    qDebug()<<videoDeviceIndex;
}
void RtmpLive::doUpdateDevice(){
    int oNums=deviceNums;
    deviceNums=vCap->getDeviceNum();
    if(oNums==deviceNums){

    }else{
        for(int i=0;i<oNums;i++){
            ui->videocomboBox->removeItem(1);
        }
        QString vname="";
        for(int i=0;i<deviceNums;i++){
            vname=QString::fromLocal8Bit("摄像头");
            vname+=QString::number(i,10);
    //        qDebug()<<vname;
            ui->videocomboBox->insertItem(i+1,vname);
        }
    }
}
void RtmpLive::doDispVideo(){
    if(runFlag==3){
        if(videoDeviceIndex>0)
            vCap->getFrame(vbuf);
        if(videoDeviceIndex==0)
            dCap->getFrame(vbuf);
        QImage srcImg(vbuf,width,height,QImage::Format_RGB888);
        ui->videolabel->setPixmap(QPixmap::fromImage(srcImg.scaled(ui->videolabel->size())));
        ui->videolabel->update();
    }
    else if(runFlag==0){
        if(vbuf)
            delete[]vbuf;
            vbuf=NULL;
    }
}
void RtmpLive::doCalTime(){
    calTime++;
    ui->timerlabel->setText(QString::number(calTime));
}
