/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#pragma once
#include "PullDataObj.h"

class G711FramedLiveSource:public FramedSource
{
	public:
	G711FramedLiveSource(UsageEnvironment& env, CRtspSourceDataObj *pObj);
	~G711FramedLiveSource(void);

protected:
	virtual void doGetNextFrame();

private:
	CRtspSourceDataObj *     m_pPullDataObj;
	long long          m_startPts ;
	timeval            m_start_tval;

	unsigned char      m_pBuffer[150000];
	unsigned long long m_dLastTimeStamp;
	unsigned long long             m_dFirstTimeStamp;
	struct timeval                 m_fFirstPresentationTime;
	struct timeval fTempTime;

};



