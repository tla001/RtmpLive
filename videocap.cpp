#include "videocap.h"

VideoCap::VideoCap(){
    VI=new videoInput;
    runFlag=false;
    stopFlag=false;
    width=0;
    height=0;
    device=0;
    deviceNums=0;
    frameSize=0;
    buffer=NULL;
}
VideoCap::~VideoCap(){
    if(!buffer)
        delete[] buffer;
    delete VI;
}
void VideoCap::run(){
    runFlag=true;
    worker();
    runFlag=false;
}
int VideoCap::getDeviceNum(){
    deviceNums = VI->listDevices();
    return deviceNums;
}

int VideoCap::init(int width, int height, int device){
    this->device=device;
    VI->setUseCallback(true);
    VI->setIdealFramerate(this->device,60);

    VI->setupDevice(device, width, height);
    this->width = VI->getWidth(device);
    this->height = VI->getHeight(device);
//    VI->showSettingsWindow(0);//该语句可以显示视频设置窗口，可以去掉
    frameSize = VI->getSize(device);
    buffer=new unsigned char[frameSize];

    return frameSize;
}

void VideoCap::worker(){
    while(!stopFlag){
        VI->getPixels(device, buffer, true, true);//不能使用videoInput内置的翻转，内存冲突，暂未解决
        emit capFinish();
    }
}
void VideoCap::getFrame(unsigned char *&framebuff){
    if(framebuff==NULL)
        return;
    memcpy(framebuff,buffer,frameSize);
}
void VideoCap::doCap(){
    stopFlag=false;
    this->start();
}
void VideoCap::doStop(){
    stopFlag=true;
    while(runFlag)
        msleep(10);
    VI->stopDevice(device);
}
/*yuv格式转换为rgb格式*/
void VideoCap::YUV422toRGB24(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;
    for(in = 0; in < width * height * 2; in += 4) {
        pixel_16 =
        yuv[in + 3] << 24 |
        yuv[in + 2] << 16 |
        yuv[in + 1] <<  8 |
        yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
}
int VideoCap::convert_yuv_to_rgb_pixel(int y, int u, int v)
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;
    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = r * 220 / 256;
    pixel[1] = g * 220 / 256;
    pixel[2] = b * 220 / 256;
    return pixel32;
}
