/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/


#include "H264LiveVideoServerMediaSubssion.h"
#include "H264FramedLiveSource.h"


H264LiveVideoServerMediaSubssion::~H264LiveVideoServerMediaSubssion(void)
{
	//m_pPullDataObj->SubClientRef();
}

H264LiveVideoServerMediaSubssion* H264LiveVideoServerMediaSubssion::createNew( UsageEnvironment& env,CRtspSourceDataObj *pObj )
{
	return new H264LiveVideoServerMediaSubssion(env,pObj);
}

H264LiveVideoServerMediaSubssion::H264LiveVideoServerMediaSubssion( UsageEnvironment& env, CRtspSourceDataObj *pObj ):\
	OnDemandServerMediaSubsession(env, true)
{
	m_pSourceDataObj = pObj;
	m_pSDPLine = NULL;
	m_pDummyRTPSink = NULL;
}
char const * H264LiveVideoServerMediaSubssion::getAuxSDPLine2(RTPSink * rtpSink, FramedSource * inputSource)
{
	if (m_pSDPLine)
	{
		return m_pSDPLine;
	}
	m_pDummyRTPSink = rtpSink;

	//mp_dummy_rtpsink->startPlaying(*source, afterPlayingDummy, this);  
	m_pDummyRTPSink->startPlaying(*inputSource, 0, 0);

	chkForAuxSDPLine(this);

	m_done = 0;

	envir().taskScheduler().doEventLoop(&m_done);

	m_pSDPLine = strdup(m_pDummyRTPSink->auxSDPLine());

	m_pDummyRTPSink->stopPlaying();

	return m_pSDPLine;
}
FramedSource* H264LiveVideoServerMediaSubssion::createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate )
{
	printf("=====================createNewStreamSource\n");
	m_pSourceDataObj->ClearData();
	FramedSource* pFrameSource = new H264FramedLiveSource(envir(),m_pSourceDataObj);
	estBitrate = 500;
	OutPacketBuffer::increaseMaxSizeTo(100000); // bytes
	FramedSource* result = H264VideoStreamFramer::createNew(envir(),pFrameSource);
	return result;
}

RTPSink* H264LiveVideoServerMediaSubssion::createNewRTPSink( Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource )
{
	RTPSink* result = NULL;
	result = H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
	return result;
}

void H264LiveVideoServerMediaSubssion::startStream( unsigned clientSessionId, void* streamToken, TaskFunc* rtcpRRHandler, void* rtcpRRHandlerClientData, unsigned short& rtpSeqNum, unsigned& rtpTimestamp, ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler, void* serverRequestAlternativeByteHandlerClientData )
{
	//m_pPullDataObj->AddClientRef();

	OnDemandServerMediaSubsession::startStream(clientSessionId, streamToken, rtcpRRHandler, rtcpRRHandlerClientData, rtpSeqNum,
		rtpTimestamp, serverRequestAlternativeByteHandler, serverRequestAlternativeByteHandlerClientData);
}
void H264LiveVideoServerMediaSubssion::afterPlayingDummy(void * ptr)
{
	H264LiveVideoServerMediaSubssion * This = (H264LiveVideoServerMediaSubssion *)ptr;

	This->m_done = 0xff;
}

void H264LiveVideoServerMediaSubssion::chkForAuxSDPLine(void * ptr)
{
	H264LiveVideoServerMediaSubssion * This = (H264LiveVideoServerMediaSubssion *)ptr;

	This->chkForAuxSDPLine1();
}

void H264LiveVideoServerMediaSubssion::chkForAuxSDPLine1()
{
	if (NULL == m_pDummyRTPSink)
	{
		return;
	}
	if (m_pDummyRTPSink->auxSDPLine())
	{
		m_done = 0xff;
	}
	else
	{
		double delay = 1000.0 / (25);  // ms  
		int to_delay = delay * 1000;  // us  

		nextTask() = envir().taskScheduler().scheduleDelayedTask(to_delay, chkForAuxSDPLine, this);
	}
}

void H264LiveVideoServerMediaSubssion::deleteStream(unsigned clientSessionId, void*& streamToken)
{
	OnDemandServerMediaSubsession::deleteStream(clientSessionId, streamToken);
	{
		char sInfo[1000] = { 0 };
		sprintf(sInfo, "==============================H264LiveVideoServerMediaSubssion::deleteStream:0x%x,  fDestinationsHashTable size : %d\n", this, fDestinationsHashTable->numEntries());
		printf(sInfo);
	}
	
	if (fDestinationsHashTable->IsEmpty())
	{

		m_pSourceDataObj->SetVideoDataState(false);
	}
}
