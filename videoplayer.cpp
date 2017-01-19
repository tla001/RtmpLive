#include "videoplayer.h"
#include "ui_videoplayer.h"
#include <QIcon>
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>


videoPlayer::videoPlayer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::videoPlayer)
{
    ui->setupUi(this);
    setWindowTitle( QString::fromLocal8Bit("多媒体播放器"));
    setWindowIcon(QIcon(":/icos/images/stream.ico"));
    fileName="";
    url="";
    totalTime=0;
    currentTime=0;
    volume=100;
    ui->volumeSlider->setValue(volume);
    runFlag=false;
    streamFlag=false;
    stateInfo[0]=QString::fromLocal8Bit("空闲状态");
    stateInfo[1]=QString::fromLocal8Bit("正在播放");
    stateInfo[2]=QString::fromLocal8Bit("暂停播放");
    stateInfo[3]=QString::fromLocal8Bit("播放结束");
    stateInfo[4]=QString::fromLocal8Bit("");
    ui->urlEdit->setText("rtmp://live.tla001.cn/vod/test.flv");
//    ui->urlEdit->setText("rtmp://live.hkstv.hk.lxdns.com/live/hks");

    timer=new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(doTimerSlot()));
    player=new VLCPlayer;
    player->init(ui->videolabel->winId());

//    fileName="cuc_ieschool.flv";


    doUpdateStateInfo(0);
    doUpdateFileInfo(QString::fromLocal8Bit("未打开任何文件或地址"));
    ui->volumeSlider->hide();
    ui->processSlider->setEnabled(false);
    doSelectInput();
    connect(ui->streamcheckBox,SIGNAL(stateChanged(int)),this,SLOT(doSelectInput()));
    connect(ui->volumeButton,SIGNAL(clicked(bool)),this,SLOT(doVolumeButton()));
    connect(ui->startButton,SIGNAL(clicked(bool)),this,SLOT(doStartButton()));
    connect(ui->stopButton,SIGNAL(clicked(bool)),this,SLOT(doStopButton()));
    connect(ui->openFileButton,SIGNAL(clicked(bool)),this,SLOT(doOpenFileButton()));
    connect(ui->volumeSlider,SIGNAL(valueChanged(int)),this,SLOT(doVolumeChanged()));
    connect(ui->volumeSlider,SIGNAL(sliderMoved(int)),this,SLOT(doVolumeChanged()));
//    connect(ui->processSlider,SIGNAL(valueChanged(int)),this,SLOT(doProcessChanged()));
    connect(ui->processSlider,SIGNAL(sliderMoved(int)),this,SLOT(doProcessChanged()));
    connect(ui->speedButton,SIGNAL(clicked(bool)),this,SLOT(doSpeedButton()));

}

videoPlayer::~videoPlayer()
{
    doStopButton();
    delete ui;
}
/*
 * 0--停止
 * 1--运行
 * 2--暂停
 */
void videoPlayer::doStartButton(){
    if(runFlag==0){
        if(streamFlag){
            fileName="";
            url=ui->urlEdit->text();
            if(url.isEmpty()){
                QMessageBox::warning(this,"null","URL is empty");
                ui->urlEdit->setFocus();
            }else{
                 doUpdateFileInfo(QString::fromLocal8Bit("网络:")+url);
                 player->playUrl(url.toUtf8().data());
                 doUpdateStateInfo(1);
 //                totalTime=player->getTotalTime();
 //                qDebug()<<"total "<<totalTime;
                 timer->start(100);
            }
        }else{
            if(fileName.isEmpty()){
                QMessageBox::warning(this,"No file","Please open a file");
                return;
            }else{
                player->playFile(fileName.toUtf8().data());
                doUpdateStateInfo(1);
//                totalTime=player->getTotalTime();
//                qDebug()<<"total "<<totalTime;
                timer->start(100);
            }
        }
        runFlag=1;
        ui->startButton->setText(QString::fromLocal8Bit("暂停"));
        ui->processSlider->setEnabled(true);
    }else if(runFlag==1){
        timer->stop();
        runFlag=2;
        ui->startButton->setText(QString::fromLocal8Bit("播放"));
        player->pause(true);
        doUpdateStateInfo(2);
    }else if(runFlag==2){
        timer->start(100);
        runFlag=1;
        ui->startButton->setText(QString::fromLocal8Bit("暂停"));
        player->pause(false);
        doUpdateStateInfo(1);
    }
}
void videoPlayer::doStopButton(){
    doUpdateStateInfo(0);
    totalTime=0;
    timer->stop();
    player->stop();
    runFlag=0;
    ui->startButton->setText(QString::fromLocal8Bit("播放"));
    ui->processSlider->setEnabled(false);
    ui->processSlider->setValue(0);
     QTime t(0,0,0,0);
     ui->currenttimelabel->setText(t.toString("hh:mm:ss"));
     ui->totaltimelabel->setText(t.toString("hh:mm:ss"));
}
void videoPlayer::doVolumeButton(){
    static bool stats=false;
    if(stats==false){
        stats=true;
        ui->volumeSlider->show();
    }else{
        stats=false;
        ui->volumeSlider->hide();
    }
}
void videoPlayer::doOpenFileButton(){
    fileName=QFileDialog::getOpenFileName(this,"Open File",QDir::currentPath(),
     "Common media formats(*.avi *mkv *.wmv *.wmp *.wm *.asf *flv *.rm *.ram *.rmvb *.ra *.mpg *.mpeg *.mpe *.m1v *.m2v *.mpv2 \
     *.mp2v *.dat *.mp4 *.m4v *.m4p *.vob *.ac3 *.dts *.mov *.qt *.mr *.3gp *.3gpp *.3g2 *.3gp2; *.swf *.ogg *.wma *.wav \
    *.mid *.midi *.mpa *.mp2 *.mp3 *.m1a *.m2a *.m4a *.aac *.mk *.ogm *.m4b *.tp *.ts *.tpr *.pva *.pss *.wv *.m2ts *.evo);;\
    All Files(*.*) ");
    if(fileName.isEmpty()){
        QMessageBox::warning(this,"No file selected","Please select a file");
        return;
    }else{
        QFileInfo info(fileName);
         doUpdateFileInfo(QString::fromLocal8Bit("文件:")+info.fileName());
         fileName=fileName.replace("/","\\");
         url="";
//         qDebug()<<fileName;
    }
}
void videoPlayer::doSelectInput(){
    if(ui->streamcheckBox->checkState()){
        streamFlag=true;
        ui->openFileButton->setEnabled(false);
        ui->urlEdit->setEnabled(true);
    }
    else{
       streamFlag=false;
       ui->openFileButton->setEnabled(true);
       ui->urlEdit->setEnabled(false);
    }
}
void videoPlayer::doUpdateStateInfo(int index){
    ui->stateinfolabel->clear();
    ui->stateinfolabel->setText(stateInfo[index]);
}
void videoPlayer::doUpdateFileInfo(QString info){
    ui->fileinfolabel->clear();
    ui->fileinfolabel->setText(info);
}
void videoPlayer::doTimerSlot(){
    if(player->getPlayerStat()==libvlc_Ended)
        doPlayEnd();
    currentTime=player->getCurrentTime();
    totalTime=player->getTotalTime();
    QTime total_time(totalTime/3600000, (totalTime/60000)%60, (totalTime/1000)%60,0);
   QTime current_time(currentTime/3600000, (currentTime/60000)%60, (currentTime/1000)%60,0);//传进来的time参数代表了当前的时间
   ui->currenttimelabel->setText(current_time.toString("hh:mm:ss"));
   ui->totaltimelabel->setText(total_time.toString("hh:mm:ss"));
//    qDebug()<<"cur "<<currentTime;
//    qDebug()<<"total "<<totalTime;
//    qDebug()<<(int)(currentTime*100/totalTime);
    ui->processSlider->setValue((int)(currentTime*1000/totalTime));
}
void videoPlayer::doVolumeChanged(){
    player->setVolume(ui->volumeSlider->value());
}
void videoPlayer::doProcessChanged(){
    player->setPlayerPostion((float)ui->processSlider->value()/1000);
}
void videoPlayer::doPlayEnd(){
    doStopButton();
}
void videoPlayer::doSpeedButton(){
    float speed= QInputDialog::getDouble(this,"speed set","speed");
     if(speed>2.0||speed<0){
         QMessageBox::critical(this,"Error","speed must be in 0.0~2.0");
         doSpeedButton();
     }else{
         player->setSpeed(speed);
     }
}
