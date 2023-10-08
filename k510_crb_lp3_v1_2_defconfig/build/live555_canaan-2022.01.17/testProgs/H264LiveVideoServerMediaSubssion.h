/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/


#pragma once
#include <OnDemandServerMediaSubsession.hh>
#include "PullDataObj.h"

class H264LiveVideoServerMediaSubssion: public OnDemandServerMediaSubsession
{
public:
	static H264LiveVideoServerMediaSubssion* createNew(UsageEnvironment& env,CRtspSourceDataObj *pObj);
	~H264LiveVideoServerMediaSubssion(void);

protected:
	virtual char const * getAuxSDPLine2(RTPSink * rtpSink, FramedSource * inputSource);
	H264LiveVideoServerMediaSubssion(UsageEnvironment& env, CRtspSourceDataObj *pObj);
	virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);
	virtual void startStream(unsigned clientSessionId, void* streamToken,
		TaskFunc* rtcpRRHandler,
		void* rtcpRRHandlerClientData,
		unsigned short& rtpSeqNum,
		unsigned& rtpTimestamp,
		ServerRequestAlternativeByteHandler* serverRequestAlternativeByteHandler,
		void* serverRequestAlternativeByteHandlerClientData);

	static void afterPlayingDummy(void * ptr);

	static void chkForAuxSDPLine(void * ptr);
	void chkForAuxSDPLine1();
	virtual void deleteStream(unsigned clientSessionId, void*& streamToken);

private:
	CRtspSourceDataObj*            m_pSourceDataObj;

	char * m_pSDPLine;
	RTPSink * m_pDummyRTPSink;
	char m_done;
};

