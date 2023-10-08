/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/


#include "H264AACSourceData.h"

CH264AACSourceData::CH264AACSourceData(void)
{
	m_bGetVideoData = false; 
	m_bGetAudioData = false;

	m_VideoFrameBufCtrl.SetID(0);
	m_AudioFrameBufCtrl.SetID(1);

	memset(m_pAudioAdtsHead, 0, 7);
}


CH264AACSourceData::~CH264AACSourceData(void)
{
}


bool CH264AACSourceData::GetVideoFrame(unsigned char* &pBuffer, int &nLen, unsigned long long &dTimestamp)
{
	//if (!m_bGetVideoData)
	//{
	//	m_bGetVideoData = true;
	//}

	bool bRet = m_VideoFrameBufCtrl.PopBuffer(nLen, pBuffer,dTimestamp,false);
	return bRet;
}

bool CH264AACSourceData::GetAudioFrame(unsigned char* &pBuffer, int &nLen, unsigned long long &dTimestamp)
{

	//if (!m_bGetAudioData)
	//{
	//	m_bGetAudioData = true;
	//}
	//ÒôÆµ²»×èÈû
	bool bRet = m_AudioFrameBufCtrl.PopBuffer(nLen, pBuffer,dTimestamp,false);
	return bRet;
}



bool CH264AACSourceData::AddVideoFrame(unsigned      char*pBuffer,int nLen,unsigned long long dTimestamp )
{
	if (m_bGetVideoData)
	{
		m_VideoFrameBufCtrl.PullBuffer(pBuffer, nLen,dTimestamp,false);
	}
	return true;
}

bool CH264AACSourceData::AddAudioFrame(unsigned char*pBuffer,int nLen,unsigned long long dTimestamp )
{
	if (m_pAudioAdtsHead[0] == 0 || m_pAudioAdtsHead[1] == 0)
	{
		memcpy(m_pAudioAdtsHead, pBuffer, 7);
	}

	if (m_bGetAudioData)
	{
		//ÒôÆµ²»×èÈû
		m_AudioFrameBufCtrl.PullBuffer(pBuffer, nLen,dTimestamp,false);
	}
	return true;
}

bool CH264AACSourceData::SetVideoDataState(bool bget)
{
	m_bGetVideoData = bget;
	if (!m_bGetVideoData)
	{
		m_VideoFrameBufCtrl.Reset();
	}

	{
		char sInfo[100] = { 0 };
		sprintf(sInfo, "rtspserver->SetVideoDataState:%d,0x%x\n", bget,this);
		printf(sInfo);
	}
	return true;
}

bool CH264AACSourceData::SetAuidoDataState(bool bget)
{
	m_bGetAudioData = bget;
	if (!m_bGetAudioData)
	{
		m_AudioFrameBufCtrl.Reset();
	}

	{
		char sInfo[100] = { 0 };
		sprintf(sInfo, "rtspserver->SetAuidoDataState:%d,0x%x\n", bget,this);
		printf(sInfo);
	}
	return true;
}

void CH264AACSourceData::GetAdtsHead(char* &pAdtsHead)
{
	pAdtsHead = m_pAudioAdtsHead;
}

bool CH264AACSourceData::ClearData()
{
	m_VideoFrameBufCtrl.Reset();
	m_AudioFrameBufCtrl.Reset();
	m_bGetAudioData = false;
	m_bGetVideoData = false;
	return true;
}
