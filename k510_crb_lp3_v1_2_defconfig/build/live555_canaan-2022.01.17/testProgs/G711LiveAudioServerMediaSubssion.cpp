/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "G711LiveAudioServerMediaSubssion.h"
#include "G711FramedLiveSource.h"
#include"IRtspServer.h"

G711LiveAudioServerMediaSubssion::~G711LiveAudioServerMediaSubssion(void)
{
	
}

G711LiveAudioServerMediaSubssion* G711LiveAudioServerMediaSubssion::createNew( UsageEnvironment& env, CRtspSourceDataObj *pObj,const RTSP_AUDIO_INFO& audioInfo)
{
	return new G711LiveAudioServerMediaSubssion(env,pObj,audioInfo);
}

G711LiveAudioServerMediaSubssion::G711LiveAudioServerMediaSubssion( UsageEnvironment& env, CRtspSourceDataObj *pObj,const RTSP_AUDIO_INFO& audioInfo):\
	OnDemandServerMediaSubsession(env, true)
{
	m_pSourceDataObj = pObj;
	m_G711AudioInfo  = audioInfo;
}


FramedSource* G711LiveAudioServerMediaSubssion::createNewStreamSource(  unsigned clientSessionId,unsigned& estBitrate )
{
	unsigned bitsPerSecond = m_G711AudioInfo.nSamplingFrequency*m_G711AudioInfo.nBitsPerSample*m_G711AudioInfo.nNumChannels;
	estBitrate = (bitsPerSecond+500)/1000; // kbps

  FramedSource* pFrameSource = new G711FramedLiveSource(envir(), m_pSourceDataObj);
	return pFrameSource;
}

RTPSink* G711LiveAudioServerMediaSubssion::createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource )
{
	do {
			char const* mimeType;
			unsigned char payloadFormatCode = rtpPayloadTypeIfDynamic; // by default, unless a static RTP payload type can be used
			if (em_audio_type_g711a == m_G711AudioInfo.audioType)
			{
				mimeType = "PCMA";
				if (m_G711AudioInfo.nSamplingFrequency == 8000 && m_G711AudioInfo.nNumChannels == 1)
				{
					payloadFormatCode = 8; // a static RTP payload type
				}
			}
			else if (em_audio_type_g711u == m_G711AudioInfo.audioType)
			{
				mimeType = "PCMU";
				if (m_G711AudioInfo.nSamplingFrequency == 8000 && m_G711AudioInfo.nNumChannels == 1)
				{
					payloadFormatCode = 0; // a static RTP payload type
				}
			}
			else
			{
				break;
			}
    
    return SimpleRTPSink::createNew(envir(), rtpGroupsock,
				    payloadFormatCode, m_G711AudioInfo.nSamplingFrequency,
				    "audio", mimeType, m_G711AudioInfo.nNumChannels);
  } while (0);

  // An error occurred:
  return NULL;
}

void G711LiveAudioServerMediaSubssion::deleteStream(unsigned clientSessionId, void*& streamToken)
{
	OnDemandServerMediaSubsession::deleteStream(clientSessionId, streamToken);
	{
		char sInfo[1000] = { 0 };
		sprintf(sInfo, "==============================G711LiveAudioServerMediaSubssion::deleteStream:%p,fDestinationsHashTable size:%d\n", this, fDestinationsHashTable->numEntries());
		printf(sInfo);
	}

	if (fDestinationsHashTable->IsEmpty())
	{
		m_pSourceDataObj->SetAuidoDataState(false);
	}
}










