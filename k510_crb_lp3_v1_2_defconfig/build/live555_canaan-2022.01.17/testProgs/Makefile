PREFIX = /usr/local
INCLUDES =-I../UsageEnvironment/include -I../groupsock/include -I../liveMedia/include -I../BasicUsageEnvironment/include
# Default library filename suffixes for each library that we link with.  The "config.*" file might redefine these later.
libliveMedia_LIB_SUFFIX = $(LIB_SUFFIX)
libBasicUsageEnvironment_LIB_SUFFIX = $(LIB_SUFFIX)
libUsageEnvironment_LIB_SUFFIX = $(LIB_SUFFIX)
libgroupsock_LIB_SUFFIX = $(LIB_SUFFIX)
##### Change the following for your environment:
CROSS_COMPILE?=		riscv64-linux-
COMPILE_OPTS =		$(INCLUDES) -I. -O2 -DNO_OPENSSL=1 -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64
C =			c
C_COMPILER =		$(CROSS_COMPILE)gcc
C_FLAGS =		$(COMPILE_OPTS)
CPP =			cpp
CPLUSPLUS_COMPILER =	$(CROSS_COMPILE)g++
CPLUSPLUS_FLAGS =	$(COMPILE_OPTS) -Wall -DBSD=1
OBJ =			o
LINK =			$(CROSS_COMPILE)g++ -o
LINK_OPTS =		
CONSOLE_LINK_OPTS =	$(LINK_OPTS)
LIBRARY_LINK =		$(CROSS_COMPILE)ar cr 
LIBRARY_LINK_OPTS =	$(LINK_OPTS)
LIB_SUFFIX =			a
LIBS_FOR_CONSOLE_APPLICATION = 
LIBS_FOR_GUI_APPLICATION =
EXE =
##### End of variables to change
TEST_APPS = VideoStreamerFile$(EXE)
TEST_LIBS = VideoStreamer.$(LIB_SUFFIX)

ALL = $(TEST_APPS) $(TEST_LIBS) 
all: $(ALL)

extra:	testGSMStreamer$(EXE)

.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<
.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

HELPER_OBJS = announceURL.$(OBJ)

VIDEO_STREAMER_OBJS = VideoStreamer.$(OBJ)
VIDEO_STREAMER_FILE_OBJS = VideoStreamerFile.$(OBJ)
RTSP_OBJS = DataBufCtrlEX.o H264AACSourceData.o H264FramedLiveSource.o H264LiveVideoServerMediaSubssion.o  G711FramedLiveSource.o G711LiveAudioServerMediaSubssion.o RtspServerEX.o



announceURL.$(CPP): 			   announceURL.hh
VideoStreamer.$(CPP):                       announceURL.hh VideoStreamer.hh
VideoStreamerFile.$(CPP):                       announceURL.hh

openRTSP.$(CPP):	playCommon.hh
playCommon.$(CPP):	playCommon.hh
playSIP.$(CPP):		playCommon.hh



DataBufCtrlEX.$(CPP):DataBufCtrlEX.h
H264AACSourceData.$(CPP):H264AACSourceData.h
H264FramedLiveSource.$(CPP):H264FramedLiveSource.h
H264LiveVideoServerMediaSubssion.$(CPP):H264LiveVideoServerMediaSubssion.h
G711FramedLiveSource$(CPP):G711FramedLiveSource.h
G711LiveAudioServerMediaSubssion.$(CPP):G711LiveAudioServerMediaSubssion.h
RtspServerEX.$(CPP):announceURL.hh IRtspServer.h RtspServerEX.h H264AACSourceData.h H264LiveVideoServerMediaSubssion.h G711FramedLiveSource.h G711LiveAudioServerMediaSubssion.h


USAGE_ENVIRONMENT_DIR = ../UsageEnvironment
USAGE_ENVIRONMENT_LIB = $(USAGE_ENVIRONMENT_DIR)/libUsageEnvironment.$(libUsageEnvironment_LIB_SUFFIX)
BASIC_USAGE_ENVIRONMENT_DIR = ../BasicUsageEnvironment
BASIC_USAGE_ENVIRONMENT_LIB = $(BASIC_USAGE_ENVIRONMENT_DIR)/libBasicUsageEnvironment.$(libBasicUsageEnvironment_LIB_SUFFIX)
LIVEMEDIA_DIR = ../liveMedia
LIVEMEDIA_LIB = $(LIVEMEDIA_DIR)/libliveMedia.$(libliveMedia_LIB_SUFFIX)
GROUPSOCK_DIR = ../groupsock
GROUPSOCK_LIB = $(GROUPSOCK_DIR)/libgroupsock.$(libgroupsock_LIB_SUFFIX)
LOCAL_LIBS =	$(LIVEMEDIA_LIB) $(GROUPSOCK_LIB) \
		$(BASIC_USAGE_ENVIRONMENT_LIB) $(USAGE_ENVIRONMENT_LIB)
LIBS =			$(LOCAL_LIBS) $(LIBS_FOR_CONSOLE_APPLICATION)

VideoStreamer.$(LIB_SUFFIX):$(VIDEO_STREAMER_OBJS) $(RTSP_OBJS)  $(HELPER_OBJS) $(LOCAL_LIBS)
	$(LIBRARY_LINK)$@ $(LIBRARY_LINK_OPTS) $(VIDEO_STREAMER_OBJS) $(RTSP_OBJS) $(HELPER_OBJS) $(LOCAL_LIBS)
VideoStreamerFile$(EXE):$(VIDEO_STREAMER_FILE_OBJS)  $(HELPER_OBJS) $(LOCAL_LIBS)
	$(LINK)$@ $(CONSOLE_LINK_OPTS) $(VIDEO_STREAMER_FILE_OBJS) $(HELPER_OBJS) $(LOCAL_LIBS)

clean:
	-rm -rf *.$(OBJ) $(ALL) $(LIBS) core *.core *~ include/*~

install: $(ALL)
	  install -d $(DESTDIR)$(PREFIX)/bin
	  install -m 755 $(ALL) $(DESTDIR)$(PREFIX)/bin

##### Any additional, platform-specific rules come here:
