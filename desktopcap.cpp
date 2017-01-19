#include "desktopcap.h"

DesktopCap::DesktopCap(){
    desk = QApplication::desktop();
    screen =QApplication::primaryScreen();
    timer=new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(worker()));
}
DesktopCap::~DesktopCap(){
//    delete desk;
//    delete screen;
}

int DesktopCap::init(int width, int height){
    this->width=width;
    this->height=height;
    size=width*height*3;
    return  size;
}

void DesktopCap::doCap(){
    timer->start(40);
    buf=new unsigned char[size];
}
void DesktopCap::doStop(){
    timer->stop();
    msleep(20);
    delete [] buf;
}
void DesktopCap::worker(){
    if(screen){
        pix=screen->grabWindow(desk->winId(),0,0,desk->width(),desk->height()).scaled(width,height);
        img=pix.toImage().convertToFormat(QImage::Format_RGB888);
        memcpy(buf,img.constBits(),size);
        emit capFinish();
    }
}
void DesktopCap::getFrame(unsigned char *&framebuff){
    if(framebuff==NULL)
        return;
    memcpy(framebuff,buf,size);
}
