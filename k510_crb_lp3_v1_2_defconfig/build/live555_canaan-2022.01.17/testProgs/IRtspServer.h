
/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#pragma once
#include <pthread.h>

enum RTSP_AUDIO_TYPE
{
	em_audio_type_g711a = 0,
	em_audio_type_g711u
};

struct RTSP_AUDIO_INFO
{
	RTSP_AUDIO_TYPE              audioType;     //��Ƶ��������
	int                          nBitsPerSample;//����λ��
	int                          nSamplingFrequency;//������
	int                          nNumChannels;  //ͨ����
};


class IRtspServerEX
{
public:
	static IRtspServerEX*   CreateRTSPServerEX();
	virtual ~IRtspServerEX() {};
	virtual int     Init(short sPort,bool bTransferAudio=false) = 0;//��ʼ���˿ں�
	virtual void    SetAudioInfo(const RTSP_AUDIO_INFO& audioInfo) = 0;
	virtual int     CreateStreamUrl(char const* streamName) = 0;//��������ַ
	virtual bool    PushVideoData(unsigned char *pBuf, int nlen, unsigned long long dTime) = 0;  //��Ƶ����(I֡��SPS��PPS���ڲ��Զ�ʶ����Ƶ��������)
	virtual bool    PushAudioData(unsigned char *pBuf, int nlen, unsigned long long dTime) = 0;
	virtual void    GetThreadId(pthread_t *tid) = 0;
};





