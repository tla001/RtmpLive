/*
 * RTMPPushFlv.cpp
 *
 *  Created on: Jan 11, 2017
 *      Author: tla001
 */

#include "RTMPPushFlv.h"
#include "sockInit.h"
#include "rtmplive.h"
#include <QDebug>
extern RtmpLive *w;


RTMPPushFlv::RTMPPushFlv() {
	// TODO Auto-generated constructor stub

	 start_time = 0;
	 now_time = 0;

	 pre_frame_time = 0;
	 lasttime = 0;
	 b_next_is_key = 1;
	 pre_tag_size = 0;

	 type = 0;
	 datalength = 0;
	 timestamp = 0;
    stopFlag=false;
    fp=NULL;
     rtmp = RTMP_Alloc();
     timer=new QTimer();
}

RTMPPushFlv::~RTMPPushFlv() {

	// TODO Auto-generated destructor stub
	if (fp != NULL) {
		fclose(fp);
		fp = NULL;
	}
	if (rtmp != NULL) {
		RTMP_Close(rtmp);
		RTMP_Free(rtmp);
		rtmp = NULL;
	}
	if (p_file_buf != NULL) {
		free(p_file_buf);
		p_file_buf = NULL;
	}
}

int RTMPPushFlv::init(const string url,const string filename){

    rtmpUrl=url;
    inFile=filename;
//    qDebug()<<filename.c_str()<<" "<<url.c_str();
//    if(filename!=NULL)
     fp = fopen(inFile.c_str()/*"C:/Users/Administrator/rec.flv"*/, "rb");
    if (NULL == fp) {
        qDebug("open err");
        return -1;
    }
     w->doShowSendFInfo("Open file successed");
//    qDebug("here 1");
//    rtmp = RTMP_Alloc();
    RTMP_Init(rtmp);
//    qDebug("here 2");
//    InitSockets();
    //set connection timeout,default 30s
    rtmp->Link.timeout = 5;
    if (!RTMP_SetupURL(rtmp,const_cast<char*>(rtmpUrl.c_str()) /*"rtmp://live.tla001.cn/live/test1"*/)) {
//        RTMP_Log(RTMP_LOGERROR, "SetupURL Err\n");
        w->doShowSendFInfo("SetupURL Err");
//        RTMP_Free(rtmp);
        return -1;
    }
    RTMP_EnableWrite(rtmp);
    if (!RTMP_Connect(rtmp, NULL)) {
//        RTMP_Log(RTMP_LOGERROR, "Connect Err\n");
        w->doShowSendFInfo("Connect Err");
//        RTMP_Free(rtmp);
        return -1;
    }
    if (!RTMP_ConnectStream(rtmp, 0)) {
//        RTMP_Log(RTMP_LOGERROR, "ConnectStream Err\n");
        w->doShowSendFInfo("ConnectStream Err");
        RTMP_Close(rtmp);
//        RTMP_Free(rtmp);
        return -1;
    }
    //jump over FLV Header
    fseek(fp, 9, SEEK_SET);
    //jump over previousTagSizen
    fseek(fp, 4, SEEK_CUR);
//    qDebug("here 4");
     w->doShowSendFInfo("RTMP Init successed");
	return 0;
}

void RTMPPushFlv::run(){
    runFlag=true;
    worker();
    runFlag=false;
}
void RTMPPushFlv::worker(){
    start_time = 0;
    now_time = 0;

    pre_frame_time = 0;
    lasttime = 0;
    b_next_is_key = 1;
    pre_tag_size = 0;

    type = 0;
    datalength = 0;
    timestamp = 0;
	start_time = RTMP_GetTime();
    char sfshowInfo[100];
    while (!stopFlag) {
		if ((((now_time = RTMP_GetTime()) - start_time)
				< (pre_frame_time)) && b_next_is_key) {
			//wait for 1 sec if the send process is too fast
			//this mechanism is not very good,need some improvement
			if (pre_frame_time > lasttime) {
//				RTMP_LogPrintf("TimeStamp:%8lu ms\n", pre_frame_time);
                sprintf(sfshowInfo,"TimeStamp:%8lu ms", pre_frame_time);
                w->doShowSendFInfo(sfshowInfo);
                lasttime = pre_frame_time;
			}
			sleep(1);
			continue;
		}

		//jump over type
		fseek(fp, 1, SEEK_CUR);
		if (!ReadU24(&datalength, fp)) {
			break;
		}
		if (!ReadTime(&timestamp, fp)) {
			break;
		}
		//jump back
		fseek(fp, -8, SEEK_CUR);

		p_file_buf = (char *) malloc(11 + datalength + 4);
		memset(p_file_buf, 0, 11 + datalength + 4);
		if (fread(p_file_buf, 1, 11 + datalength + 4, fp) != (11 + datalength + 4)) {
			break;
		}

		pre_frame_time = timestamp;

		if (!RTMP_IsConnected(rtmp)) {
//			RTMP_Log(RTMP_LOGERROR, "rtmp is not connect\n");
            w->doShowSendFInfo("rtmp is not connect");
			break;
		}
		if (!RTMP_Write(rtmp, p_file_buf, 11 + datalength + 4)) {
//			RTMP_Log(RTMP_LOGERROR, "Rtmp Write Error\n");
            w->doShowSendFInfo("Rtmp Write Error");
			break;
		}

		free(p_file_buf);
		p_file_buf = NULL;

		if (!PeekU8(&type, fp)) {
			break;
		}
		if (0x09 == type) {
			if (fseek(fp, 11, SEEK_CUR) != 0) {
				break;
			}
			if (!PeekU8(&type, fp)) {
				break;
			}
			if (type == 0x17) {
				b_next_is_key = 1;
			} else {
				b_next_is_key = 0;
			}
			fseek(fp, -11, SEEK_CUR);
		}
	}
    doNormalExit();

    this->quit();
}
void RTMPPushFlv::doPush(){
    stopFlag=false;
	this->start();
//    timer->start(1000);
}
void RTMPPushFlv::doStop(){
    stopFlag=true;
//    timer->stop();
    while(runFlag)
        msleep(10);
    if (fp != NULL) {
       fclose(fp);
       fp = NULL;
   }
    if (rtmp != NULL) {
        RTMP_Close(rtmp);
    }
     w->doShowSendFInfo("Push Stopped");
}
void RTMPPushFlv::doNormalExit(){
    if(false==stopFlag){
        w->doShowSendFInfo("Push Finished");
        emit nornalExit();
        if (rtmp != NULL) {
            RTMP_Close(rtmp);
        }
    }
    if (fp != NULL) {
       fclose(fp);
       fp = NULL;
   }
}
