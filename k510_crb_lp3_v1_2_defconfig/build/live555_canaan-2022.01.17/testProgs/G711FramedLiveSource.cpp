/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#include"G711FramedLiveSource.h"
#include <GroupsockHelper.hh>



G711FramedLiveSource::G711FramedLiveSource(UsageEnvironment& env, CRtspSourceDataObj *pObj): FramedSource(env)
{

	m_startPts = 0;
	m_dLastTimeStamp = 0;

	m_dFirstTimeStamp = 0;

	m_pPullDataObj = pObj;
	pObj->SetAuidoDataState(true);
	printf("=============================new G711FramedLiveSource\n");
}


G711FramedLiveSource::~G711FramedLiveSource(void)
{
	printf("=============================delete G711FramedLiveSource\n");
	m_pPullDataObj->SetAuidoDataState(false);
}

void G711FramedLiveSource::doGetNextFrame()
{
	fDurationInMicroseconds = 0;
	fFrameSize = 0;
	fNumTruncatedBytes = 0;

	int nLen = fMaxSize;
	unsigned char *pBuffer = m_pBuffer;
	unsigned long long  dTimeStamp = 0;
	if (m_pPullDataObj->GetAudioFrame(pBuffer, nLen, dTimeStamp))
	{

		//if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
			if (m_dFirstTimeStamp == 0) {
				// This is the first frame, so use the current time:
			fDurationInMicroseconds = 0;
			m_dLastTimeStamp = dTimeStamp;
			m_dFirstTimeStamp = dTimeStamp;
			gettimeofday(&fPresentationTime, NULL);
			gettimeofday(&m_fFirstPresentationTime, NULL);
			



		}
		else {

#if 0
			unsigned long long lastTimeTemp = fPresentationTime.tv_sec * 1000000 + fPresentationTime.tv_usec;
			gettimeofday(&fPresentationTime, NULL);
			unsigned long long nowTimeTemp = fPresentationTime.tv_sec * 1000000 + fPresentationTime.tv_usec;
			fDurationInMicroseconds = nowTimeTemp - lastTimeTemp;
			m_dLastTimeStamp = dTimeStamp;
#else
			//unsigned differTime = dTimeStamp - m_dLastTimeStamp;
			//fDurationInMicroseconds = differTime;
			//unsigned uSeconds = fPresentationTime.tv_usec + differTime;
			//fPresentationTime.tv_sec += uSeconds / 1000000;
			//fPresentationTime.tv_usec = uSeconds % 1000000;
			//m_dLastTimeStamp = dTimeStamp;


			unsigned differTime = dTimeStamp - m_dLastTimeStamp;
			fDurationInMicroseconds = differTime;
			m_dLastTimeStamp = dTimeStamp;


			unsigned long long uSeconds = m_fFirstPresentationTime.tv_usec + (dTimeStamp - m_dFirstTimeStamp);
			long tv_sec = uSeconds / 1000000;
			long tv_usec = uSeconds % 1000000;
			fPresentationTime.tv_sec = m_fFirstPresentationTime.tv_sec + tv_sec;
			fPresentationTime.tv_usec = tv_usec;

 


			
#endif 

		}
		
		if (nLen > fMaxSize)
		{
			nLen = fMaxSize;
			fNumTruncatedBytes = nLen - fMaxSize;
		}

		//ȥ��adtsͷ 
#if 0
		//memcpy(fTo, pBuffer + 7, nLen - 7);
		//fFrameSize = nLen - 7;
#else
		memcpy(fTo, pBuffer, nLen);
		fFrameSize = nLen;
#endif 
	}
	else
	{
		//nextTask() = envir().taskScheduler().scheduleDelayedTask(20000,
		//	(TaskFunc*)FramedSource::afterGetting, this);
		//printf("audio++++++++++++++++++++++++++++++++++++++++++++++++++\n");
		//return;
		if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
			gettimeofday(&fPresentationTime, NULL);

			{
				
				printf("audio fPresentationTime timestamp = 0\n");
			}
		}
		//fDurationInMicroseconds = fuSecsPerFrame;
		nextTask() = envir().taskScheduler().scheduleDelayedTask(20000,
			(TaskFunc*)FramedSource::afterGetting, this);
		return;
		
	}

	nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
		(TaskFunc*)FramedSource::afterGetting, this);//��ʾ�ӳ�0�����ִ�� afterGetting ����
}




