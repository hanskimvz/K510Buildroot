/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include "DataBufCtrlEX.h"
#include <string.h>
#include <stdio.h>

#include<string.h>

//#define   HI_MAX_DATA_SIZE  5000000
#define   HI_MAX_VIDEO_DATA_SIZE  20000000
#define   HI_MAX_AUDIO_DATA_SIZE  500000
CDataBufCtrlEX::CDataBufCtrlEX(void)
{
	m_nBufferLen = 0;
//	memset(m_Buffer, 0, HI_MAX_DATA_SIZE);
	pthread_mutex_init(&m_FrameMutex, NULL);
	pthread_cond_init(&m_DataCond, NULL);
	m_nPushIndex = 0;
	m_nPopIndex = 0;
	m_nId = -1;
	m_Buffer = NULL;
}


CDataBufCtrlEX::~CDataBufCtrlEX(void)
{
	pthread_mutex_destroy(&m_FrameMutex);
	pthread_cond_destroy(&m_DataCond);
	if (m_Buffer != NULL)
	{
		delete[] m_Buffer;
		m_Buffer = NULL;
	}
}

void CDataBufCtrlEX::SetID(int nId)
{
	m_nId = nId;
	if (0 == nId)
	{
		m_Buffer = new unsigned char[HI_MAX_VIDEO_DATA_SIZE];
		memset(m_Buffer, 0, HI_MAX_VIDEO_DATA_SIZE);
	}
	else if (1 == nId)
	{
		m_Buffer = new unsigned char[HI_MAX_AUDIO_DATA_SIZE];
		memset(m_Buffer, 0, HI_MAX_AUDIO_DATA_SIZE);
	}
	
}

int CDataBufCtrlEX::GetBufferSize()
{
	pthread_mutex_lock(&m_FrameMutex);
	int nSize = m_nBufferLen;
	pthread_mutex_unlock(&m_FrameMutex);
	return nSize;
}

bool CDataBufCtrlEX::PullBuffer(unsigned char*pBuffer, int nSize, unsigned long long dTimestamp, bool bBlock/* = true*/)
{
	
	pthread_mutex_lock(&m_FrameMutex);
	//if (m_nBufferLen + nSize > HI_MAX_DATA_SIZE)
	if (m_nPushIndex - m_nPopIndex > 100)
	{
		m_nBufferLen = 0;
		char sInfo[1000] = { 0 };
		sprintf(sInfo, "the buffer is full,empty the buffer:%x,id:%d\n", this,m_nId);
		printf(sInfo);
		m_mapFrameTimeStamp.clear();
		m_nPopIndex = m_nPushIndex;//清空之前的数据，popindex也要清空
	}

	memcpy(m_Buffer + m_nBufferLen, pBuffer, nSize);
	m_nBufferLen += nSize;

	DATA_BUF_FRAME_INFO  Info;
	Info.nLen = nSize;
	Info.dTimeStamp = dTimestamp;
	if (m_nPushIndex < 0)
	{
		m_nPushIndex = 0;
	}
	m_mapFrameTimeStamp[m_nPushIndex++] = Info;
	
	pthread_mutex_unlock(&m_FrameMutex);
	if (bBlock)
	{
		pthread_cond_signal(&m_DataCond);
	}
	return true;
}

bool CDataBufCtrlEX::PopBuffer(int &nSize, unsigned char* &pBuffer, unsigned long long &dTimestamp, bool bBlock /*= true*/)
{

	pthread_mutex_lock(&m_FrameMutex);
	if (bBlock)
	{
		pthread_cond_wait(&m_DataCond, &m_FrameMutex);
	}

	int nTmpSize = nSize;

	if (m_nBufferLen <= 0)
	{
		nSize = 0;
		pthread_mutex_unlock(&m_FrameMutex);
		return false;
	}

	int nIndex = m_nPopIndex++;
	int nLen = m_mapFrameTimeStamp[nIndex].nLen;
	unsigned long long dTime = m_mapFrameTimeStamp[nIndex].dTimeStamp;
	int nLeftTmpSize = nLen;
	//如果当前帧大于要取得大小，则将该帧剩余部分保存到该帧中
	if (nLen > nSize)
	{

		m_mapFrameTimeStamp[nIndex].nLen = nLen - nSize;
		m_nPopIndex--;
		nLen = nSize;
		printf("=========================nlen > nSize\n");
	}
	else
	{
		//删除
		m_mapFrameTimeStamp.erase(nIndex);
	}

	nSize = nLen;
	memcpy(pBuffer, m_Buffer, nLen);
	dTimestamp = dTime;
	int nLeft = m_nBufferLen - nLen;

	memmove(m_Buffer, m_Buffer + nLen, nLeft);
	m_nBufferLen = nLeft;
	pthread_mutex_unlock(&m_FrameMutex);

	if (m_nId == 0)
	{
		if (m_nPushIndex - m_nPopIndex > 80)
		{
			char buf[100] = { 0 };
			sprintf(buf, "video=============================left:%d_%d[%d-%d]_total:%d\n", nLeftTmpSize, nTmpSize, m_nPushIndex, m_nPopIndex, m_nBufferLen);
			printf(buf);
		}


	}
	else if (1 == m_nId)
	{
		if (m_nPushIndex - m_nPopIndex > 80)
		{
			char buf[100] = { 0 };
			sprintf(buf, "audio=============================left:%d_%d[%d-%d]_total:%d\n", nLeftTmpSize, nTmpSize, m_nPushIndex, m_nPopIndex, m_nBufferLen);
			printf(buf);
		}

	}
	return true;
}

bool CDataBufCtrlEX::PopBufferAll(int &nsize, unsigned char* &pBuffer)
{
	memcpy(pBuffer, m_Buffer, m_nBufferLen);

	nsize = m_nBufferLen;
	m_nBufferLen = 0;

	return true;
}

bool CDataBufCtrlEX::Reset()
{
	pthread_mutex_lock(&m_FrameMutex);
	m_nBufferLen = 0;
	m_nPushIndex = 0;
	m_nPopIndex = 0;
	m_mapFrameTimeStamp.clear();
	pthread_mutex_unlock(&m_FrameMutex);

	return true;
}