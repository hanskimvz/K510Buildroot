/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#pragma once
#include <stdlib.h>  
#include <pthread.h>  
#include <stdint.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <map>
using namespace std;


struct DATA_BUF_FRAME_INFO
{
	int    nLen;
	unsigned long long dTimeStamp;
	DATA_BUF_FRAME_INFO()
	{
		nLen = 0;
		dTimeStamp = 0;
	}
};
typedef std::map<int, DATA_BUF_FRAME_INFO>   FRAME_TIMESTAMP_MAP;
class CDataBufCtrlEX
{
public:
	CDataBufCtrlEX(void);
	~CDataBufCtrlEX(void);
	void SetID(int nId);
	int  GetBufferSize();
	bool PullBuffer(unsigned char*pBuffer, int nSize, unsigned long long dTimestamp,bool bBlock = true);
	bool PopBuffer(int &nSize, unsigned char* &pBuffer, unsigned long long &dTimestamp, bool bBlock = true);
	bool PopBufferAll(int &nsize, unsigned char* &pBuffer);
	bool Reset();

private:
//	char     m_Buffer[HI_MAX_DATA_SIZE];
	unsigned char*    m_Buffer;
	int      m_nBufferLen;
	FRAME_TIMESTAMP_MAP    m_mapFrameTimeStamp;
	pthread_mutex_t      m_FrameMutex;
	pthread_cond_t       m_DataCond;

	int                  m_nPushIndex;
	int                  m_nPopIndex;

	int                  m_nId;

};