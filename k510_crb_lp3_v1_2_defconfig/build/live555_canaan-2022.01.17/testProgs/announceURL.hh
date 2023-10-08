/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/


#ifndef _ANNOUNCE_URL_HH
#define _ANNOUNCE_URL_HH

#ifndef _LIVEMEDIA_HH
#include "liveMedia.hh"
#endif

void announceURL(RTSPServer* rtspServer, ServerMediaSession* sms);

#endif
