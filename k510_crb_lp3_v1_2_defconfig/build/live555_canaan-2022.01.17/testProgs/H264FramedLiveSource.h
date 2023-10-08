/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#pragma once

#include "PullDataObj.h"
class H264FramedLiveSource:public FramedSource
{
public:
	H264FramedLiveSource(UsageEnvironment& env, CRtspSourceDataObj *pObj);
	~H264FramedLiveSource(void);


protected:
	virtual void doGetNextFrame();
	virtual unsigned int maxFrameSize() const;

private:
	CRtspSourceDataObj *     m_pPullDataObj;

	long long          m_startPts ;
	timeval            m_start_tval;

	struct timeval fTempTime;

	unsigned char      m_pBuffer[1500000] ;
	unsigned long long             m_dLastTimeStamp;
	unsigned long long             m_dFirstTimeStamp;
	struct timeval                 m_fFirstPresentationTime; 
	unsigned fLastPlayTime;
};

