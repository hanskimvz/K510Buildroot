/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/


#pragma once
class CRtspSourceDataObj;
class UsageEnvironment;
class RTSPServer;
#include"IRtspServer.h"
#include <pthread.h>

class CRtspServerEX:public IRtspServerEX
{
public:
	CRtspServerEX();
	~CRtspServerEX();	

	int     Init(short sPort,bool bTransferAudio=false) ;
	virtual void    SetAudioInfo(const RTSP_AUDIO_INFO& audioInfo);
	int     CreateStreamUrl(char const* streamName);
	bool    PushVideoData(unsigned char *pBuf, int nlen, unsigned long long dTime);
	bool    PushAudioData(unsigned char *pBuf, int nlen, unsigned long long dTime);
	void    GetThreadId(pthread_t *tid);

protected:
	static void* StartRtspEventLoop(void* pUser);

	int    _InitStreamUrl();

private:
	bool     m_bInitStream;
	char     m_sStreamName[256];
	static bool m_sbInitLog;
	short m_Port;
	bool  m_bInit;
	RTSPServer* m_rtspServer;
	UsageEnvironment* m_pEnv;

	CRtspSourceDataObj  *m_pSourceData;
	int          m_nCurUrlIndex;
	RTSP_AUDIO_INFO        m_audioInfo;
	bool                 m_bTransferAudio;
	pthread_t m_tid;
};

