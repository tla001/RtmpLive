#include "rtmplive.h"
#include <QApplication>
#include <stdio.h>
#include "librtmp/rtmp_sys.h"
#include "librtmp/log.h"

#pragma comment(lib,"WS2_32.lib")
RtmpLive *w;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    w=new RtmpLive;
    w->show();

    return a.exec();
}


