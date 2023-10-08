/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/


#pragma once
#include<iostream>
using namespace std;
#include<string>
#include <list>
#include<string.h>
#include<liveMedia.hh>

#define TEST_NEW  1
typedef enum tag_TrackType
{
	emTrackUnknown =0,
	emTrackVideo = 1,
	emTrackAudio,
	emTrackSubtitle,
}TRACK_TYPE;
typedef struct  tag_StreamTrack {
	// track parameters
	unsigned trackNumber;
	TRACK_TYPE trackType;
	unsigned samplingRate;
	unsigned numChannels;
	std::string mimeType;
	unsigned codecPrivateSize;
	unsigned char* codecPrivate;
	tag_StreamTrack()
	{
		trackNumber = -1;
		trackType = emTrackUnknown;
		samplingRate = 0;
		numChannels = 0;
		codecPrivateSize = 0;
		codecPrivate = NULL;
	}
}STREAM_TRACK;

typedef struct tag_FrameInfo
{
	unsigned char *pBuffer;
	int    dwSize;
	long long  pts;
	long long  dts;
	bool       bkey;

	tag_FrameInfo()
	{
		pBuffer = NULL;
		dwSize = 0;
		pts = 0;
		dts = 0;
		bkey = false;
	}

	void SetInfo(unsigned char*p1,int size1,long long pts1,long long dts1, bool key1)
	{
		if (pBuffer != NULL)
		{
			delete[] pBuffer;
			pBuffer = NULL;
		}

		pBuffer = new unsigned char[size1];
		memcpy(pBuffer,p1,size1);
		pts = pts1;
		dts = dts1;
		bkey = key1;
		dwSize = size1;
	}

	~tag_FrameInfo()
	{
		if (pBuffer != NULL)
		{
			delete[] pBuffer;
			pBuffer = NULL;
		}
		pts = 0;
		dts = 0;
		bkey = false;
		dwSize = 0;
	}


}FRAME_INFO;

typedef std::list<FRAME_INFO*> FRAME_INFO_LIST;



class CRtspSourceDataObj
{
public:
	virtual        ~CRtspSourceDataObj(){}
	virtual bool       GetVideoFrame(unsigned char* &pBuffer, int &nLen,unsigned long long &dTimestamp) = 0;
	virtual bool       GetAudioFrame(unsigned char* &pBuffer, int &nLen, unsigned long long &dTimestamp) = 0;
	virtual bool       AddVideoFrame(unsigned char*pBuffer,int nLen,unsigned long long dTimestamp) = 0;
	virtual bool       AddAudioFrame(unsigned char*pBuffer,int nLen,unsigned long long dTimestamp) = 0;
	virtual bool       SetVideoDataState(bool bget) = 0;
	virtual bool       SetAuidoDataState(bool bget) = 0;
	virtual bool       ClearData() = 0;
	virtual void       GetAdtsHead(char* &pAdtsHead) =0;

public:
	char    m_pAudioAdtsHead[7];
};

