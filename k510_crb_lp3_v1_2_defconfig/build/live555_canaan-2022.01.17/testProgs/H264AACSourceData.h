/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/


#ifndef _H264AACSOURCEDATA_H
#define _H264AACSOURCEDATA_H

#include "PullDataObj.h"

#include "DataBufCtrlEX.h"

class CH264AACSourceData:public CRtspSourceDataObj
{
public:
	CH264AACSourceData(void);
	~CH264AACSourceData(void);

	virtual bool       GetVideoFrame(unsigned char* &pBuffer,int &nLen, unsigned long long &dTimestamp);
	virtual bool       GetAudioFrame(unsigned char* &pBuffer, int &nLen,unsigned long long &dTimestamp);
	virtual bool       AddVideoFrame(unsigned char*pBuffer, int nLen, unsigned long long dTimestamp);
	virtual bool       AddAudioFrame(unsigned char*pBuffer, int nLen, unsigned long long dTimestamp);
	virtual bool       ClearData();
	virtual bool       SetVideoDataState(bool bget);
	virtual bool       SetAuidoDataState(bool bget);
	virtual void       GetAdtsHead(char* &pAdtsHead);

private:
	CDataBufCtrlEX       m_VideoFrameBufCtrl;
	CDataBufCtrlEX         m_AudioFrameBufCtrl;


	bool               m_bGetVideoData;
	bool               m_bGetAudioData;
};

#endif 
