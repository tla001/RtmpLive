
#pragma once

#include "librtmp_send264.h"
#include "sps_decode.h"

//#ifdef  _BASEFUNC_EXPORT_
//#define  BASE_API  extern "C" __declspec(dllexport)
//#else
//#define  BASE_API   __declspec(dllimport)
//#endif

//#define __STDC_CONSTANT_MACROS
//#pragma comment(lib,"winmm.lib")
//#pragma comment(lib,"psapi.lib")
extern "C" {
#include "x264.h"
#include "x264_config.h"
//#include "mp4v2\mp4v2.h"
#include "libswscale/swscale.h"
//#include "libavutil\opt.h"
#include "libavutil/imgutils.h"
}

//CRITICAL_SECTION m_Cs;// 临界区结构对象

typedef struct
{
	// rtmp object
	char*				szUrl;
	RTMP*				rtmp;
	RTMPPacket			packet;
	// faac encoder

	unsigned long       nSampleRate;
	unsigned long       nChannels;
	unsigned long       nTimeStamp;
	unsigned long       nTimeDelta;
	char*				szPcmAudio;
	unsigned long       nPcmAudioSize;
	unsigned long       nPcmAudioLen;
	char*				szAacAudio;
	unsigned long       nAacAudioSize;
}RTMPMOD_SPublishObj;

typedef unsigned long long  QWORD, ULONG64, UINT64, ULONGLONG;

class RtmpH264
{
public:
	RtmpH264(void);


	~RtmpH264()
	{
	}

	int CreatePublish(char* url, int outChunkSize, int isOpenPrintLog, int logType);
	void DeletePublish();

	int InitVideoParams(unsigned long width, unsigned long height, unsigned long fps, unsigned long bitrate, enum AVPixelFormat src_pix_fmt,bool bConstantsBitrate);
	int SendScreenCapture(BYTE * frame, unsigned long Height, unsigned long timespan);

	//int WriteVideoParams(unsigned long width, unsigned long height, unsigned long fps, unsigned long bitrate);
	//int WriteScreenCapture(BYTE * frame, unsigned long Stride, unsigned long Height, unsigned long timespan);

	void FreeEncodeParams(); //释放占用内存资源

public:
	DWORD m_startTime;
	RTMPMOD_SPublishObj rtmp_PublishObj;

	struct SwsContext * m_SwsContext;

	int m_width;	//宽度
	int m_height;	// 高度
	int	m_frameRate;	//帧率fps
	int m_bitRate;	//比特率
	int m_baseFrameSize;

	int m_audioChannel;	//声道数
	int m_audioSample;	//音频采样率

	int m_type;//0 rtmp推流模式， 1 mp4录像模式
	bool m_isCreatePublish;

	enum AVPixelFormat m_srcPicFmt;

	uint8_t *src_data[4];
	int src_linesize[4];

};

/*创建推送流
*<param name="url">推送流地址</param>
*<param name="outChunkSize">发送包大小</param>
*/
long RTMP_CreatePublish(char* url, unsigned long outChunkSize, int isOpenPrintLog, int logType);

//删除推送流
 void RTMP_DeletePublish();


/*初始化编码参数
*<param name="width">视频宽度</param>
*<param name="height">视频高度</param>
*<param name="fps">帧率</param>
*<param name="bitrate">比特率</param>
*<param name="bConstantsBitrate"> 是否恒定码率 </param>
*/
long RTMP_InitVideoParams(unsigned long width, unsigned long height, unsigned long fps, unsigned long bitrate,enum AVPixelFormat src_pix_fmt,bool bConstantsBitrate = false);


/*初始化编码参数
*<param name="frame">图片地址</param>
*<param name="Stride">图片步幅</param>
*<param name="Height">图片行高</param>
*<param name="timespan">时间戳</param>
*/
long RTMP_SendScreenCapture(char * frame,  unsigned long Height, unsigned long timespan);


/*初始化编码参数
*<param name="szBuf">音频数据地址</param>
*<param name="nBufLen">音频数据长度 单位字节</param>
*<param name="nSampleRate">采样率</param>
*<param name="nChannels">声道数</param>
*<param name="timespan">时间戳</param>
*/
long RTMP_SendAudioFrame(char* szBuf, unsigned long nBufLen, unsigned long nSampleRate, unsigned long nChannels, unsigned long timespan);


///*创建MP4文件
//*<param name="fileName">文件名</param>
//*<param name="audioSample">采样率</param>
//*<param name="audioChannel">声道数</param>
//*/
//BASE_API long RTMP_CreateMp4File(char* fileName, int audioChannel, int audioSample);
//BASE_API void RTMP_CloseMp4File();
//
///*初始化编码参数
//*<param name="width">视频宽度</param>
//*<param name="height">视频高度</param>
//*<param name="fps">帧率</param>
//*<param name="bitrate">比特率</param>
//*/
//BASE_API long RTMP_WriteVideoParams(unsigned long width, unsigned long height, unsigned long fps, unsigned long bitrate);
//
//
///*编码图像数据并写入文件
//*<param name="szBuf">音频数据地址</param>
//*<param name="nBufLen">音频数据长度 单位字节</param>
//*<param name="nSampleRate">采样率</param>
//*<param name="nChannels">声道数</param>
//*<param name="timespan">时间戳</param>
//*/
//BASE_API long RTMP_WriteScreenCapture(char * frame, unsigned long Stride, unsigned long Height, unsigned long timespan);
//
//
///*编码音频数据并写入文件
//*<param name="szBuf">音频数据地址</param>
//*<param name="nBufLen">音频数据长度 单位字节</param>
//*<param name="nSampleRate">采样率</param>
//*<param name="nChannels">声道数</param>
//*<param name="timespan">时间戳</param>
//*/
//BASE_API long RTMP_WriteAudioFrame(char* szBuf, unsigned long nBufLen, unsigned long nSampleRate, unsigned long nChannels, unsigned long timespan);
