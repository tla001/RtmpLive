#ifndef VIDEOCAP_H
#define VIDEOCAP_H
#include "videoInput.h"
#include <QThread>

class VideoCap : public QThread{
    Q_OBJECT
public:
    VideoCap();
    ~VideoCap();
    void run();
    int getDeviceNum();
    int init(int width,int height,int device);
    void worker();
    void getFrame(unsigned char *&framebuff);
    void doCap();
    void doStop();
    int convert_yuv_to_rgb_pixel(int y, int u, int v);
    void YUV422toRGB24(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);

private:
    videoInput *VI;
    unsigned char *buffer;
    bool runFlag;
    bool stopFlag;
    int width;
    int height;
    int device;
    int deviceNums;
    int frameSize;

signals:
    void capFinish();
};

#endif // VIDEOCAP_H
