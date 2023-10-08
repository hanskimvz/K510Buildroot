/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/


#include "H264FramedLiveSource.h"
#include <GroupsockHelper.hh>
#include<liveMedia.hh>
#include<BasicUsageEnvironment.hh>
H264FramedLiveSource::H264FramedLiveSource( UsageEnvironment& env,CRtspSourceDataObj* pObj) : FramedSource(env)
{	
	//printf("=============================new H264FramedLiveSource\n");
	//fDurationInMicroseconds = 40000;
	m_pPullDataObj = pObj;
	m_startPts = 0;
	fLastPlayTime = 0;
	m_dLastTimeStamp = 0;
	m_dFirstTimeStamp = 0;

	pObj->SetVideoDataState(true);
	printf("=============================new H264FramedLiveSource\n");
}


H264FramedLiveSource::~H264FramedLiveSource(void)
{
	m_pPullDataObj->SetVideoDataState(false);
	printf("=============================delete H264FramedLiveSource\n");

}

void H264FramedLiveSource::doGetNextFrame()
{
	//用光该帧的所有数据，再去用下一帧，如果当前没有数据，则framesize 设置成 0

	fDurationInMicroseconds = 0;
	fFrameSize = 0;
	fNumTruncatedBytes = 0;

	
	int nLen = fMaxSize;
	unsigned char *pBuffer = m_pBuffer;
	unsigned long long dTimeStamp = 0;
	if (m_pPullDataObj->GetVideoFrame(pBuffer, nLen, dTimeStamp))
	{

		//if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
		if (m_dFirstTimeStamp == 0) {
			// This is the first frame, so use the current time:
			fDurationInMicroseconds = 0;
			//gettimeofday(&m_fFirstPresentationTime, NULL);
			////gettimeofday(&fPresentationTime, NULL);
			//fPresentationTime = m_fFirstPresentationTime;
			m_dLastTimeStamp = dTimeStamp;
			m_dFirstTimeStamp = dTimeStamp;
			gettimeofday(&fPresentationTime, NULL);
			gettimeofday(&m_fFirstPresentationTime, NULL);

		}
		else {

			unsigned differTime = dTimeStamp - m_dLastTimeStamp;
			fDurationInMicroseconds = differTime;
			m_dLastTimeStamp = dTimeStamp;


			unsigned long long uSeconds = m_fFirstPresentationTime.tv_usec + (dTimeStamp - m_dFirstTimeStamp);
			long tv_sec = uSeconds / 1000000;
			long tv_usec = uSeconds % 1000000;
			fPresentationTime.tv_sec = m_fFirstPresentationTime.tv_sec + tv_sec;
			fPresentationTime.tv_usec = tv_usec;
		}
		
		if (nLen > fMaxSize)
		{
			nLen = fMaxSize;
			fNumTruncatedBytes = nLen - fMaxSize;
		}
		memcpy(fTo, pBuffer, nLen);
		fFrameSize = nLen;
	}
	else
	{
		if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
			gettimeofday(&fPresentationTime, NULL);
		}
		fDurationInMicroseconds = 40000;
		nextTask() = envir().taskScheduler().scheduleDelayedTask(20000,
			(TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数

		return;
	}

	//if (fFrameSize == 0)
	//{
	//	printf("video fFrameSize is 0\n");
	//}

	nextTask() = envir().taskScheduler().scheduleDelayedTask( 0,
		(TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数


}

unsigned int H264FramedLiveSource::maxFrameSize() const
{
//	return 150000;

	return 1500000;
}
