/* Copyright (c) 2022, Canaan Bright Sight Co., Ltd

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/


#include <liveMedia.hh>

#include <BasicUsageEnvironment.hh>
#include "announceURL.hh"
#include <GroupsockHelper.hh>

#include "../preconfig.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>

UsageEnvironment* env;
char const* inputFileName = "test.264";
H264VideoStreamFramer* videoSource;
RTPSink* videoSink;

__off64_t pPhybuffer = 0;
uint8_t* pVirbuffer = 0;
uint64_t datasize = 0;
uint32_t frame_index = 0;
EventTriggerId eventTriggerId;
char volatile* watchVariable = 0;
char eventLoopWatchVariable = 0;
Boolean pingpong = 0;
void *shared_main_vAddr;

void play(); // forward
void play1(); // forward
int VideoStreamerFile(long bufAddr, uint32_t bufSize);

int main(int argc, char *argv[])
{
    FILE *fp;
    char ch;
    uint32_t cnt = 0,i = 0;
    uint32_t filesize, pagesize, syncsize;
    uint8_t *map_src_pic, *pic_vaddr;
    int32_t ret;
    errno = 0;
    int fd_h264 = 0, fd_mem = 0, fd_share_memory = 0, fd_sysctl = 0;
    struct share_memory_alloc_align_args allocAlignMem_264;

    if(argc < 1){
        printf("usage: %s <filename>\n", argv[0]);
    }
    char *filename = argv[1];

    fd_mem = open(DEV_NAME_DDR,O_RDWR|O_SYNC); 
    if(fd_mem < 0){
        printf("%s>Open %s Error!\n",__FUNCTION__, DEV_NAME_DDR);
        return -1;
    }

    fd_share_memory = open(SHARE_MEMORY_DEV,O_RDWR | O_SYNC);
    if(fd_share_memory < 0){
        printf("%s>Open %s Error!\n",__FUNCTION__, SHARE_MEMORY_DEV);
        close(fd_mem);
        return -1; 
    }

    printf("file_name: %s\n", argv[1]);
    
    if( (fp=fopen(filename,"r+b")) == NULL ){
        printf("Cannot open file, press any key to exit!\n");
    }

    fseek(fp,0L,SEEK_END);
    filesize = ftell(fp);

    fseek(fp,0,SEEK_SET);

    allocAlignMem_264.size = (filesize + 0xfff) & (~0xfff);
    allocAlignMem_264.alignment = MEMORY_TEST_BLOCK_ALIGN;
    allocAlignMem_264.phyAddr = 0;

    ret = ioctl(fd_share_memory, SHARE_MEMORY_ALIGN_ALLOC, &allocAlignMem_264);
    if(ret < 0){
        printf("alloc ddr for yuv Error:%d\n",ret);
        return -1;
    }
    // printf("264 phy addr = 0x%08x\/%d\n",allocAlignMem_264.phyAddr,allocAlignMem_264.phyAddr);
    
    map_src_pic = (uint8_t*)mmap(NULL, filesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd_mem, allocAlignMem_264.phyAddr);
    // printf("map_src_pic = 0x%x\n",map_src_pic);

    pic_vaddr = map_src_pic;
    // printf("pic_vaddr = 0x%x\n",pic_vaddr);
    for(i = 0; i < filesize; i++)
    {
        ch = fgetc(fp);
        
        if(ch == EOF)
        {
            printf("end of file\n");
        }            
        else
        {            
            *pic_vaddr = ch;
            pic_vaddr++;
        }
    }
     
    printf("write file done\n");

    VideoStreamerFile(allocAlignMem_264.phyAddr,allocAlignMem_264.size);

    ioctl(fd_share_memory, SHARE_MEMORY_FREE, allocAlignMem_264.phyAddr);
    
    close(fd_share_memory);
    close(fd_mem);
    fclose(fp);

    return 0;
}

int VideoStreamerFile(long bufAddr, uint32_t bufSize) 
{
	pPhybuffer = (__off64_t)bufAddr;
  datasize = bufSize;
  printf("pPhybuffer = 0x%08x\n",pPhybuffer);
  printf("datasize = 0x%08x\n",datasize);
  shared_main_vAddr = NULL;

  /*open device node*/
  fd_h264 = open(DEV_NAME_H264, O_RDWR|O_SYNC);
  if (fd_h264 < 0){
      printf("%s>Open %s Error!\n", __FUNCTION__, DEV_NAME_H264);
      return -1;
  }

  fd_mem = open(DEV_NAME_DDR,O_RDWR|O_SYNC); 
  if(fd_mem < 0){
      printf("%s>Open %s Error!\n", __FUNCTION__, DEV_NAME_DDR);
      close(fd_h264);
      return -1;
  }

  fd_share_memory = open(SHARE_MEMORY_DEV,O_RDWR | O_SYNC);
  if(fd_share_memory < 0){
      printf("Open %s Error!\n",SHARE_MEMORY_DEV);
      close(fd_share_memory);
      close(fd_h264);
      return -1;
  }

  pVirbuffer = (uint8_t *)mmap(NULL, datasize, PROT_READ|PROT_WRITE, MAP_SHARED, fd_mem, pPhybuffer);
  // printf("pVirbuffer = 0x%08x\n",pVirbuffer);

  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  // Create 'groupsocks' for RTP and RTCP:
  struct sockaddr_storage destinationAddress;
  destinationAddress.ss_family = AF_INET;
  ((struct sockaddr_in&)destinationAddress).sin_addr.s_addr = chooseRandomIPv4SSMAddress(*env);
  // Note: This is a multicast address.  If you wish instead to stream
  // using unicast, then you should use the "testOnDemandRTSPServer"
  // test program - not this test program - as a model.

  const unsigned short rtpPortNum = 18888;
  const unsigned short rtcpPortNum = rtpPortNum+1;
  const unsigned char ttl = 255;

  const Port rtpPort(rtpPortNum);
  const Port rtcpPort(rtcpPortNum);

  Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
  rtpGroupsock.multicastSendOnly(); // we're a SSM source
  Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
  rtcpGroupsock.multicastSendOnly(); // we're a SSM source

  // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
  OutPacketBuffer::maxSize = 3000000;//zxj, 100000;
  videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);

  // Create (and start) a 'RTCP instance' for this RTP sink:
  const unsigned estimatedSessionBandwidth = 500; // in kbps; for RTCP b/w share
  const unsigned maxCNAMElen = 100;
  unsigned char CNAME[maxCNAMElen+1];
  gethostname((char*)CNAME, maxCNAMElen);
  CNAME[maxCNAMElen] = '\0'; // just in case
  RTCPInstance* rtcp
  = RTCPInstance::createNew(*env, &rtcpGroupsock,
			    estimatedSessionBandwidth, CNAME,
			    videoSink, NULL /* we're a server */,
			    True /* we're a SSM source */);
  // Note: This starts RTCP running automatically

  RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554);
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }
  ServerMediaSession* sms
    = ServerMediaSession::createNew(*env, "testStream", "testStream", /* cuiyan, filename */
		   "Session streamed by \"testH264VideoStreamer\"",
					   True /*SSM*/);
  sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
  rtspServer->addServerMediaSession(sms);
  announceURL(rtspServer, sms);

  // Start the streaming:
  *env << "Beginning streaming...\n";
  play();

  env->taskScheduler().doEventLoop(); // does not return


  printf("stop task\n");

  ioctl(fd_share_memory, SHARE_MEMORY_FREE, pPhybuffer);
  close(fd_share_memory);
  close(fd_mem);
  close(fd_h264);
  printf("end\n");
  return 0; // only to prevent compiler warning
}

void afterPlaying(void* /*clientData*/) {
  *env << "...done reading from buffer\n";
  videoSink->stopPlaying();
  Medium::close(videoSource);
  // Note that this also closes the input file that this source read from.

  // Start playing once again:
  play();
}

void play() {
  printf("playing...\n");
  // Open the input file as a 'byte-stream file source':
  ByteStreamMemoryBufferSource* bufferSource
    = ByteStreamMemoryBufferSource::createNew(*env, pVirbuffer, datasize, false);
  if (bufferSource == NULL) {
    *env << "Unable to open buffer \"" << pVirbuffer
         << "\" as a byte-stream buffer source\n";
    exit(1);
  }

  bufferSource->setSharedAddr(shared_main_vAddr);

  FramedSource* videoES = bufferSource;

  // Create a framer for the Video Elementary Stream:
  videoSource = H264VideoStreamFramer::createNew(*env, videoES);

  // Finally, start playing:
  *env << "Beginning to read from buffer...\n";
  videoSink->startPlaying(*videoSource, afterPlaying, videoSink);
}

