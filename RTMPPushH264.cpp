/*
 * RTMPPushH264.cpp
 *
 *  Created on: Jan 12, 2017
 *      Author: tla001
 */

#include "RTMPPushH264.h"
#include "librtmp/log.h"
#include <time.h>
#include "rtmplive.h"
#include <QDebug>

extern RtmpLive *w;
PushStream::PushStream(int widths,int heigths,enum AVPixelFormat format){
//	this->rtmpUrl=url;
	this->width=widths;
	this->height=heigths;
    this->src_pix_fmt=format;
	this->baseFrameSzie=width*height;
	this->buffer=new char[baseFrameSzie*3];

//	this->fps=25;
	this->rate=200;
	this->outSize=2048;
	tick=0;
	tick_gap=1000/this->fps;
	nowTime=0;
	lastTime=0;
	frameIndex=0;
    stopFlag=false;
    runFlag=false;
}
PushStream::~PushStream(){
	delete []buffer;
}

void PushStream::init(string url,int fps){
    this->rtmpUrl=url;
    this->fps=fps;
	RTMP_CreatePublish(const_cast<char*>(rtmpUrl.c_str()),outSize,1,RTMP_LOGINFO);
	RTMP_InitVideoParams(width,height,fps,rate,src_pix_fmt,false);
}
void PushStream::worker(){
    tick=0;
    tick_gap=1000/this->fps;
    nowTime=0;
    lastTime=0;
    frameIndex=0;
    while(!stopFlag){
        lastTime=RTMP_GetTime();
        if(frameIndex!=0){
            RTMP_SendScreenCapture((char*)buffer,height,tick);
        }
        switch(src_pix_fmt){
            case AV_PIX_FMT_YUV420P:
                //memcpy(buffer,ct.yuv420Frame,baseFrameSzie*3/2);
                buffer=(char *)w->vbuf;
                break;
            case AV_PIX_FMT_YUV422P:
                buffer=(char *)w->vbuf;
                break;
            case AV_PIX_FMT_RGB24:
                buffer=(char *)w->vbuf;
                break;
            default:
                printf("Not supports this format \n");
                break;
            }
        tick +=tick_gap;
        nowTime=RTMP_GetTime();
        sleepTime=tick_gap-nowTime+lastTime;
        printf("tick_gap %d nowTime %d lastTime %d sleep time %d\n",tick_gap,nowTime,lastTime,sleepTime);
        if(sleepTime>0)
            usleep(sleepTime*1000);
        frameIndex++;
    }
}
void PushStream::run(){
    runFlag=true;
    worker();
    runFlag=false;
}
void PushStream::doPush(){
    stopFlag=false;
	this->start();
}
void PushStream::dowork(char *buf){
	lastTime=RTMP_GetTime();
	if(frameIndex!=0){
			RTMP_SendScreenCapture((char*)buffer,height,tick);
			printf("send frame index -- %d\n",frameIndex);
		}
    switch(src_pix_fmt){
        case AV_PIX_FMT_YUV420P:
            memcpy(buffer,buf,baseFrameSzie*3/2);
            break;
        case AV_PIX_FMT_YUV422P:
            memcpy(buffer,buf,baseFrameSzie*2);
            break;
        case AV_PIX_FMT_RGB24:
            memcpy(buffer,buf,baseFrameSzie*3);
            break;
        default:
            printf("Not supports this format \n");
            break;
        }
    tick +=tick_gap;
    nowTime=RTMP_GetTime();
    sleepTime=tick_gap-nowTime+lastTime;
    printf("tick_gap %d nowTime %d lastTime %d sleep time %d\n",tick_gap,nowTime,lastTime,sleepTime);
    if(sleepTime>0){
        usleep(sleepTime*1000);
    }

    frameIndex++;
}
void PushStream::doStop(){
    stopFlag=true;
    while(runFlag)
        msleep(10);
    RTMP_DeletePublish();
}
