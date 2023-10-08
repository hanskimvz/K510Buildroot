/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/


#include "RtspServerEX.h"
#include "VideoStreamer.hh"
#include "H264AACSourceData.h"
#include "H264LiveVideoServerMediaSubssion.h"
#include "G711LiveAudioServerMediaSubssion.h"
#include<liveMedia.hh>
#include<BasicUsageEnvironment.hh>
#include"announceURL.hh"

IRtspServerEX* IRtspServerEX::CreateRTSPServerEX()
{
	IRtspServerEX *pRtspServerEX = new CRtspServerEX;
	return pRtspServerEX;
}


bool CRtspServerEX::m_sbInitLog = false;
CRtspServerEX::CRtspServerEX()
{
	m_pSourceData = NULL;
	m_nCurUrlIndex = -1;
	m_bInit = false;
	m_rtspServer = NULL;
	m_Port = 0;
	m_pEnv = NULL;
	memset(m_sStreamName, 0, 256);
	m_bInitStream = false;
	m_bTransferAudio = false;
}


CRtspServerEX::~CRtspServerEX()
{
}


int CRtspServerEX::CreateStreamUrl(char const* streamName)
{
	strcpy(m_sStreamName, streamName);


	return _InitStreamUrl();

}



bool CRtspServerEX::PushVideoData(unsigned char *pBuf, int nlen, unsigned long long dTime)
{

	if (m_pSourceData == NULL)
	{
		return false;
	}
	return m_pSourceData->AddVideoFrame(pBuf, nlen, dTime);
}

bool CRtspServerEX::PushAudioData(unsigned char *pBuf, int nlen, unsigned long long dTime)
{
	if (m_pSourceData == NULL || !m_bTransferAudio)
	{
		return false;
	}
	return m_pSourceData->AddAudioFrame(pBuf, nlen, dTime);
}


void* CRtspServerEX::StartRtspEventLoop(void* pUser)
{
	CRtspServerEX *pThis = (CRtspServerEX*)pUser;
	pThis->m_pEnv->taskScheduler().doEventLoop(); // does not return
	return NULL;
}



int CRtspServerEX::_InitStreamUrl()
{
	m_nCurUrlIndex++;
	m_pSourceData = new CH264AACSourceData();

	char const* descriptionString = "Session streamed by \"testOnDemandRTSPServer\"";
	ServerMediaSession* sms = ServerMediaSession::createNew(*m_pEnv, m_sStreamName, m_sStreamName, descriptionString);


	//OutPacketBuffer::maxSize = 10000000;
	OutPacketBuffer::maxSize = 10000000;//max framesize
	sms->addSubsession(H264LiveVideoServerMediaSubssion
		::createNew(*m_pEnv, m_pSourceData));//修改为自己实现的servermedia  H264LiveVideoServerMediaSubssion

	if (m_bTransferAudio)
	{
		sms->addSubsession(G711LiveAudioServerMediaSubssion
		::createNew(*m_pEnv, m_pSourceData,m_audioInfo));
	}

	m_rtspServer->addServerMediaSession(sms);

	announceURL(m_rtspServer, sms);

	//	s_pEnv<< "create url:rtsp://ip:" << CRtspServer::s_Port << "/" << streamName << "\n"; 
	m_bInitStream = true;
	return m_nCurUrlIndex;
}

int CRtspServerEX::Init(short sPort,bool bTransferAudio)
{
	if (m_bInit)
	{
		return -1;
	}

	m_bInit = true;
	m_Port = sPort;
	m_bTransferAudio = bTransferAudio;

	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	m_pEnv = BasicUsageEnvironment::createNew(*scheduler);

	UserAuthenticationDatabase* authDB = NULL;
	m_rtspServer = RTSPServer::createNew(*m_pEnv, sPort, authDB,30);
	if (m_rtspServer == NULL) {
		*m_pEnv << "Failed to create RTSP server: " << m_pEnv->getResultMsg() << "\n";
		return -1;
		//exit(1);
	}

	*m_pEnv << "create rtspserver:" << sPort << "success" << "\n";





	OutPacketBuffer::maxSize = 10000000;//max framesize
	//CreateThread(NULL, 0, StartRtspEventLoop, (LPVOID)NULL, 0, NULL);
	
	pthread_create(&m_tid, NULL, StartRtspEventLoop, this);

  char name[12]; 
  sprintf(name, "rtsp%d", sPort);
  pthread_setname_np(m_tid, name);
  
	return 0;
}

void    CRtspServerEX::SetAudioInfo(const RTSP_AUDIO_INFO& audioInfo)
{
	m_audioInfo = audioInfo;
}

void CRtspServerEX::GetThreadId(pthread_t *tid)
{
  *tid = m_tid;
}


