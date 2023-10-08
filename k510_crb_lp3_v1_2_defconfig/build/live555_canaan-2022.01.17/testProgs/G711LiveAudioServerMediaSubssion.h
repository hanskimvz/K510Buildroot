/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#pragma once
#include <OnDemandServerMediaSubsession.hh>
#include "PullDataObj.h"
#include"IRtspServer.h"
class G711LiveAudioServerMediaSubssion: public OnDemandServerMediaSubsession
{
public:
	static G711LiveAudioServerMediaSubssion* createNew(UsageEnvironment& env, CRtspSourceDataObj *pObj,const RTSP_AUDIO_INFO& audioInfo);
	~G711LiveAudioServerMediaSubssion(void);
protected:
	G711LiveAudioServerMediaSubssion(UsageEnvironment& env, CRtspSourceDataObj *pObj,const RTSP_AUDIO_INFO& audioInfo);
	virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);
	virtual void deleteStream(unsigned clientSessionId, void*& streamToken);
	
private:
	CRtspSourceDataObj*          m_pSourceDataObj;
	RTSP_AUDIO_INFO              m_G711AudioInfo;


};


