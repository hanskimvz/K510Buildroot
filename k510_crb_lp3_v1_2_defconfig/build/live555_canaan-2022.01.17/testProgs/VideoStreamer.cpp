/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/


#include <liveMedia.hh>

#include <BasicUsageEnvironment.hh>
#include "announceURL.hh"
#include <GroupsockHelper.hh>

#include "VideoStreamer.hh"
#include "../preconfig.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#define EVENT_TRIGGER    0 
#define EVENT_LOOP       1
#define EVENT_ONCE       0
// #define VIRTUAL_ADDR     1
#define DEBUG_STEP       0

void *shared_main_vAddr;

static UsageEnvironment* env;
static char const* inputFileName = "test.264";
static H264VideoStreamFramer* videoSource;
static RTPSink* videoSink;

TaskScheduler* scheduler;
struct sockaddr_storage destinationAddress;

const unsigned short rtpPortNum = 18888;
const unsigned short rtcpPortNum = rtpPortNum+1;
const unsigned char ttl = 255;

const Port rtpPort(rtpPortNum);
const Port rtcpPort(rtcpPortNum);

Groupsock* rtpGroupsock;
Groupsock* rtcpGroupsock;

const unsigned estimatedSessionBandwidth = 500; // in kbps; for RTCP b/w share
const unsigned maxCNAMElen = 100;
unsigned char CNAME[maxCNAMElen+1];

RTCPInstance* rtcp;
RTSPServer* rtspServer;
ServerMediaSession* sms;

// ByteStreamMemoryBufferSource* bufferSource;

__off64_t pPhybuffer = 0;
uint8_t* pVirbuffer = 0;
uint32_t datasize = 0;
uint32_t frame_index = 0;
EventTriggerId eventTriggerId;
char volatile* watchVariable = 0;
char volatile eventLoopWatchVariable = 0;
static Boolean pingpong = 0;
static Boolean first_time = 1;
double FRAMERATE = NULL;

#ifdef SYNC_WITH_ENC
static unsigned int bufRP = 0;
static unsigned int bufWP = 0;
static unsigned int BufSize = 0;
static unsigned int total_size=0;
#endif

void play(); // forward
void play1(); // forward
void endof_test(); //forward

int VideoStreamer_init(double framerate)
{
  // printf("VideoStreamer_init\n");
  // Begin by setting up our usage environment:
  scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  // Create 'groupsocks' for RTP and RTCP:
  destinationAddress.ss_family = AF_INET;
  ((struct sockaddr_in&)destinationAddress).sin_addr.s_addr = chooseRandomIPv4SSMAddress(*env);
  // Note: This is a multicast address.  If you wish instead to stream
  // using unicast, then you should use the "testOnDemandRTSPServer"
  // test program - not this test program - as a model.

  rtpGroupsock = new Groupsock(*env, destinationAddress, rtpPort, ttl);
  rtpGroupsock->multicastSendOnly(); // we're a SSM source
  rtcpGroupsock = new Groupsock(*env, destinationAddress, rtcpPort, ttl);
  rtcpGroupsock->multicastSendOnly(); // we're a SSM source

  // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
  OutPacketBuffer::maxSize = 3000000;//zxj, 100000;
  videoSink = H264VideoRTPSink::createNew(*env, rtpGroupsock, 96);

  // Create (and start) a 'RTCP instance' for this RTP sink:
  gethostname((char*)CNAME, maxCNAMElen);
  CNAME[maxCNAMElen] = '\0'; // just in case
  rtcp
  = RTCPInstance::createNew(*env, rtcpGroupsock,
			    estimatedSessionBandwidth, CNAME,
			    videoSink, NULL /* we're a server */,
			    True /* we're a SSM source */);
  // Note: This starts RTCP running automatically

  rtspServer = RTSPServer::createNew(*env, 8554);
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }
  sms
    = ServerMediaSession::createNew(*env, "testStream", "testStream", 
		   "Session streamed by \"testH264VideoStreamer\"",
					   True /*SSM*/);
  sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
  rtspServer->addServerMediaSession(sms);
  announceURL(rtspServer, sms);

  eventLoopWatchVariable = 0; 

  if(framerate){
    FRAMERATE = framerate;
  }
  
  // printf("VideoStreamer_init done\n");
  return 0;
}

int VideoStreamer_deinit()
{
  pVirbuffer = NULL;
  datasize = 0;
  FRAMERATE = NULL;
  eventLoopWatchVariable = 1;

  return 0;
}
// int test_api(int argc, char** argv) 
int VideoStreamer(long bufAddr, uint32_t bufSize, void *shared_vAddr) 
{
#ifdef SYNC_WITH_ENC
  BufSize = bufSize;
#endif

  pVirbuffer = (uint8_t*)(bufAddr);
  datasize = bufSize;
  shared_main_vAddr = shared_vAddr;
  printf("pVirbuffer = %p\n",pVirbuffer);
  printf("BufSize = %d\n", bufSize);

  // Start the streaming:
  *env << "Beginning streaming...\n";
  play();

  env->taskScheduler().doEventLoop(&eventLoopWatchVariable);

  //printf("stop task\n");

  printf("end\n");
  return 0; // only to prevent compiler warning
}

void afterPlaying(void* /*clientData*/) {
  *env << "...done reading from buffer\n";
  videoSink->stopPlaying();
  Medium::close(videoSource);

#ifdef SYNC_WITH_ENC
  bufRP += datasize;
  if(bufRP >= BufSize)
  {
    bufRP -= BufSize;
  }
#endif

  // Note that this also closes the input file that this source read from.
  // Start playing once again:
  play();
}

void play() {
  printf("playing...\n");
  unsigned char *pData = pVirbuffer;

#ifdef SYNC_WITH_ENC
  unsigned int *vddr;
 
  vddr = (unsigned int *)shared_main_vAddr;

  do
  {
    bufWP = *(vddr + 1);
    
    if(bufWP > bufRP)
    {
      datasize = bufWP - bufRP;
    }
    else if(bufWP == bufRP)
    {
      datasize = 0;
    }
    else
    {
      datasize = BufSize - bufRP;
      printf("------------------------------------------------------\n");
      break;
    }
    if(datasize >= BANK_SIZE)
    {
      break;
    }
    usleep(10*1000);
  }while(1);
  
  pData = pVirbuffer + bufRP;
  printf("bufWP %d, bufRP %d, datasize %d, pData %p\n", bufWP, bufRP, datasize, pData);
#endif 

  // Open the input file as a 'byte-stream file source':
  ByteStreamMemoryBufferSource* bufferSource
  // bufferSource
    = ByteStreamMemoryBufferSource::createNew(*env, pData, (uint64_t)datasize, false);
  if (bufferSource == NULL) {
    *env << "Unable to open buffer \"" << pVirbuffer
         << "\" as a byte-stream buffer source\n";
    exit(1);
  }

  bufferSource->setSharedAddr(shared_main_vAddr);

  FramedSource* videoES = bufferSource;

  // Create a framer for the Video Elementary Stream:
  if(FRAMERATE){
    printf("%s > have framerate param: %f\n", __FUNCTION__, FRAMERATE);
    videoSource = H264VideoStreamFramer::createNew(*env, videoES, FRAMERATE);
  }
  else{
    printf("%s > have no framerate param\n", __FUNCTION__);
    videoSource = H264VideoStreamFramer::createNew(*env, videoES);
  }

  // Finally, start playing:
  *env << "Beginning to read from buffer...\n";
  videoSink->startPlaying(*videoSource, afterPlaying, videoSink);
  printf("playing done\n");
}

void endof_test()
{
  printf("endof_test\n");
  eventLoopWatchVariable = 1;
}



