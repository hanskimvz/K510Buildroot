/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2022 Live Networks, Inc.  All rights reserved.
// A class for streaming data from a (static) memory buffer, as if it were a file.
// Implementation

#include "ByteStreamMemoryBufferSource.hh"
#include "GroupsockHelper.hh"
#include "../testProgs/VideoStreamer.hh"

////////// ByteStreamMemoryBufferSource //////////

ByteStreamMemoryBufferSource*
ByteStreamMemoryBufferSource::createNew(UsageEnvironment& env,
					u_int8_t* buffer, u_int64_t bufferSize,
					Boolean deleteBufferOnClose,
					unsigned preferredFrameSize,
					unsigned playTimePerFrame) {
  if (buffer == NULL) return NULL;

  return new ByteStreamMemoryBufferSource(env, buffer, bufferSize, deleteBufferOnClose, preferredFrameSize, playTimePerFrame);
}

ByteStreamMemoryBufferSource::ByteStreamMemoryBufferSource(UsageEnvironment& env,
							   u_int8_t* buffer, u_int64_t bufferSize,
							   Boolean deleteBufferOnClose,
							   unsigned preferredFrameSize,
							   unsigned playTimePerFrame)
  : FramedSource(env), fBuffer(buffer), fBufferSize(bufferSize), fCurIndex(0), fDeleteBufferOnClose(deleteBufferOnClose),
    fPreferredFrameSize(preferredFrameSize), fPlayTimePerFrame(playTimePerFrame), fLastPlayTime(0),
    fLimitNumBytesToStream(False), fNumBytesToStream(0) {
    total_read_size = 0;
}

ByteStreamMemoryBufferSource::~ByteStreamMemoryBufferSource() {
  if (fDeleteBufferOnClose) delete[] fBuffer;
}

void ByteStreamMemoryBufferSource::setSharedAddr(void *addr) {
  shared_main_vAddr = addr;
}

void ByteStreamMemoryBufferSource::seekToByteAbsolute(u_int64_t byteNumber, u_int64_t numBytesToStream) {
  fCurIndex = byteNumber;
  if (fCurIndex > fBufferSize) fCurIndex = fBufferSize;

  fNumBytesToStream = numBytesToStream;
  fLimitNumBytesToStream = fNumBytesToStream > 0;
}

void ByteStreamMemoryBufferSource::seekToByteRelative(int64_t offset, u_int64_t numBytesToStream) {
  int64_t newIndex = fCurIndex + offset;
  if (newIndex < 0) {
    fCurIndex = 0;
  } else {
    fCurIndex = (u_int64_t)offset;
    if (fCurIndex > fBufferSize) fCurIndex = fBufferSize;
  }

  fNumBytesToStream = numBytesToStream;
  fLimitNumBytesToStream = fNumBytesToStream > 0;
}

//#define RTSP_INPUT_DUMP

#ifdef RTSP_INPUT_DUMP
#include <sys/ioctl.h>
static FILE *dump_file = NULL;
static int start_dump=1;
#endif

void ByteStreamMemoryBufferSource::doGetNextFrame() {
  if (fCurIndex >= fBufferSize || (fLimitNumBytesToStream && fNumBytesToStream == 0)) {
    handleClosure();
    return;
  }

  // Try to read as many bytes as will fit in the buffer provided (or "fPreferredFrameSize" if less)
  fFrameSize = fMaxSize;
  if (fLimitNumBytesToStream && fNumBytesToStream < (u_int64_t)fFrameSize) {
    fFrameSize = (unsigned)fNumBytesToStream;
  }
  if (fPreferredFrameSize > 0 && fPreferredFrameSize < fFrameSize) {
    fFrameSize = fPreferredFrameSize;
  }

  if (fCurIndex + fFrameSize > fBufferSize) {
    fFrameSize = (unsigned)(fBufferSize - fCurIndex);
  }

  memmove(fTo, &fBuffer[fCurIndex], fFrameSize);

#ifdef RTSP_INPUT_DUMP
  if(dump_file == NULL)
  {
    if((dump_file=fopen("rtsp_dump.264","w+b")) == NULL )
    {
      printf("Cannot open output file!\n");
    }
  }
  if(start_dump == 1)
  {
    fwrite(&fBuffer[fCurIndex], 1, fFrameSize, dump_file);
    if(total_read_size > 30000000)
    {
      fclose(dump_file);
      start_dump = 0;
      printf("------------------------------------------------------\n");
    }
  }
#endif

  fCurIndex += fFrameSize;
  fNumBytesToStream -= fFrameSize;

  // Set the 'presentation time':
  if (fPlayTimePerFrame > 0 && fPreferredFrameSize > 0) {
    if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
      // This is the first frame, so use the current time:
      gettimeofday(&fPresentationTime, NULL);
    } else {
      // Increment by the play time of the previous data:
      unsigned uSeconds	= fPresentationTime.tv_usec + fLastPlayTime;
      fPresentationTime.tv_sec += uSeconds/1000000;
      fPresentationTime.tv_usec = uSeconds%1000000;
    }

    // Remember the play time of this data:
    fLastPlayTime = (fPlayTimePerFrame*fFrameSize)/fPreferredFrameSize;
    fDurationInMicroseconds = fLastPlayTime;
  } else {
    // We don't know a specific play time duration for this data,
    // so just record the current time as being the 'presentation time':
    gettimeofday(&fPresentationTime, NULL);
  }


  if(shared_main_vAddr != NULL)
  {
    unsigned int *vddr;
    unsigned int enc_size;
    
    vddr = (unsigned int *)shared_main_vAddr;
    enc_size = *vddr;
    total_read_size += fFrameSize;
    if(total_read_size >= enc_size)
    {
      printf("RTSP ERROR: enc_size %d, total_read_size %d, fFrameSize %d\n", enc_size, total_read_size, fFrameSize);
    }
  }

  // Inform the downstream object that it has data:
  FramedSource::afterGetting(this);
}
