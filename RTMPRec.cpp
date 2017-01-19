/*
 * RTMPRec.cpp
 *
 *  Created on: Jan 11, 2017
 *      Author: tla001
 */

#include "RTMPRec.h"
#include "sockInit.h"
#include "rtmplive.h"
#include <QDebug>
extern RtmpLive *w;
RTMPRec::RTMPRec() {
	// TODO Auto-generated constructor stub
	bufSize=1024*1024*10;
	buf=new char[bufSize];
	countSize=0;
	b_live_stream=true;
    rtmp=RTMP_Alloc();
}

RTMPRec::~RTMPRec() {
	// TODO Auto-generated destructor stub
	 if (fp != NULL) {
	        fclose(fp);
	        fp = NULL;
	    }

	if (buf != NULL) {
		delete[] buf;
		buf = NULL;
	}
//	CleanupSockets();
	if (rtmp != NULL) {
		RTMP_Close(rtmp);
		RTMP_Free(rtmp);
		rtmp = NULL;
	}
}

int RTMPRec::init(const string url,const string filename){
    rtmpUrl=url;
    outFile=filename;
	fp=fopen(outFile.c_str(),"wb");
	if(NULL==fp){
        //RTMP_LogPrintf("Open File Error.\n");
       w->doShowDownInfo("Open File Error");
		return -1;
	}
    w->doShowDownInfo("\nOpen File Successed");
//	InitSockets();
//    rtmp=RTMP_Alloc();
	RTMP_Init(rtmp);
	//set connection timeout,default 30s
	rtmp->Link.timeout=10;
	if (!RTMP_SetupURL(rtmp,const_cast<char*>(rtmpUrl.c_str()))) {
            //RTMP_Log(RTMP_LOGERROR, "SetupURL Err\n");
             w->doShowDownInfo("SetupURL Err");
	        return -1;
	    }
    w->doShowDownInfo("SetupURL Successed");
	if (b_live_stream) {
		rtmp->Link.lFlags |= RTMP_LF_LIVE;
	}

	//1hour
    RTMP_SetBufferMS(rtmp, 3600 * 100);

	if (!RTMP_Connect(rtmp, NULL)) {
//		RTMP_Log(RTMP_LOGERROR, "Connect Err\n");
        w->doShowDownInfo("Connect Err");
		return -1;
	}

	if (!RTMP_ConnectStream(rtmp, 0)) {
//		RTMP_Log(RTMP_LOGERROR, "ConnectStream Err\n");
        w->doShowDownInfo("ConnectStream Err");
		RTMP_Close(rtmp);
		return -1;
	}
    stopFlag=false;
    return 0;
}
void RTMPRec::run(){
    runFlag=true;
	worker();
    runFlag=false;
}
void RTMPRec::worker(){
    int nread;
    while ((nread = RTMP_Read(rtmp, buf, bufSize)) != 0&&!stopFlag) {
		fwrite(buf, 1, (size_t)nread, fp);
		memset(buf,0,bufSize);
		countSize += nread;
        sprintf(w->dshowInfo,"Receive: %5dByte, Total: %5.2fkB", nread, countSize * 1.0 / 1024);
//        RTMP_LogPrintf("Receive: %5dByte, Total: %5.2fkB\n", nread, countSize * 1.0 / 1024);
    }
    if(stopFlag)
        this->quit();
}
void RTMPRec::doSave(){
	this->start();
}
void RTMPRec::doStop(){
    stopFlag=true;
    countSize=0;
    while(runFlag)
        msleep(10);
    if (fp != NULL) {
       fclose(fp);
       fp = NULL;
//       qDebug("close fp");
   }
//    CleanupSockets();
    if (rtmp != NULL) {
//        qDebug("close rtmp");
        RTMP_Close(rtmp);
    }
}
