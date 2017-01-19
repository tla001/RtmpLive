/*
 * RTMPRec.h
 *
 *  Created on: Jan 11, 2017
 *      Author: tla001
 */

#ifndef RTMPREC_H_
#define RTMPREC_H_
#include <QThread>
extern "C"{
#include <stdio.h>
#include <stdlib.h>
}
#include <iostream>
#include <string>

#include "librtmp/rtmp.h"
#include "librtmp/log.h"


using namespace std;

class RTMPRec :public QThread{
public:
    explicit RTMPRec();
	virtual ~RTMPRec();
    int init(const string url,const string filename);
	void run();
	void worker();
	void doSave();
    void doStop();


private:
	string rtmpUrl;
	string outFile;
	int bufSize;
	char *buf;
	FILE *fp;
	long countSize;
    bool b_live_stream;

	RTMP *rtmp;
    bool stopFlag;
    bool runFlag;

};

#endif /* RTMPREC_H_ */
