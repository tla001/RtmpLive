/*
 * RTMPPushH264.h
 *
 *  Created on: Jan 12, 2017
 *      Author: tla001
 */

#ifndef RTMPPUSHH264_H_
#define RTMPPUSHH264_H_

#include <QThread>
#include "RtmpH264.h"
#include <signal.h>
#include <string>


using namespace std;
class PushStream :public QThread {
public:
    explicit PushStream(int width,int heigth,enum AVPixelFormat format);
	~PushStream();
    void init(string url,int fps);
	void run();
    void worker();
	void dowork(char *buf);
	void doPush();
    void doStop();
private:
	string rtmpUrl;
	int width;
	int height;
	int outSize;
	enum AVPixelFormat src_pix_fmt;
	int baseFrameSzie;
	char *buffer;

	int fps;
	int rate;
	unsigned int tick;
	unsigned int tick_gap;
	uint32_t nowTime,lastTime ;
	int sleepTime;
	int frameIndex;

    bool stopFlag;
    bool runFlag;

};

#endif /* RTMPPUSHH264_H_ */
