/*
 * Copyright (c) 2015, Alessandro Saccoia. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
//
//  AudioFormat_CoreAudio.cpp
//
//  Created by Alessandro Saccoia on 1/2/13.
//  Refactored by Alessandro Saccoia on 1/19/15.
//
//  For formats see Apple docs
//  https://developer.apple.com/library/mac/documentation/musicaudio/reference/CoreAudioDataTypesRef/Reference/reference.html#//apple_ref/doc/constant_group/Audio_Data_Format_Identifiers

//  for the awful looking this awful looking method is Apple's:
// https://developer.apple.com/library/mac/documentation/musicaudio/reference/CoreAudioDataTypesRef/Reference/reference.html#//apple_ref/c/func/FillOutASBDForLPCM

#include "AudioFormat_CoreAudio.hpp"

#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioToolbox/ExtendedAudioFile.h>

#include <iostream>


namespace asu {
namespace assets {

namespace {
  // fill the AudioStreamBasicDescription depending on the params
  bool fillASBD(AudioStreamBasicDescription& asbd_,
    const AudioFormatTypes& format,
    const float& sr_,
    const int& channels_) {
    if (format == ASU_FORMAT_WAV) {
      asbd_.mSampleRate         = sr_;
      asbd_.mFormatID           = kAudioFormatLinearPCM;
      asbd_.mFormatFlags        = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
      asbd_.mChannelsPerFrame   = channels_;
      asbd_.mBytesPerPacket     = channels_ * 2;
      asbd_.mBytesPerFrame      = channels_ * 2;
      asbd_.mFramesPerPacket    = 1;
      asbd_.mBitsPerChannel     = 16;
      asbd_.mReserved           = 0;
    } else if (format == ASU_FORMAT_AIFF) {
      asbd_.mSampleRate         = sr_;
      asbd_.mFormatID           = kAudioFormatLinearPCM;
      asbd_.mFormatFlags        = kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
      asbd_.mChannelsPerFrame   = channels_;
      asbd_.mBytesPerPacket     = channels_ * 2;
      asbd_.mBytesPerFrame      = channels_ * 2;
      asbd_.mFramesPerPacket    = 1;
      asbd_.mBitsPerChannel     = 16;
      asbd_.mReserved           = 0;
    } else if (format == ASU_FORMAT_MP3) {
      // this doesn't work for encoding!
      // http://stackoverflow.com/questions/2450103/operation-could-not-be-completed-avaudiorecorder-iphone-sdk
      asbd_.mSampleRate         = sr_;
      asbd_.mFormatID           = kAudioFormatMPEGLayer3;
//      asbd_.mFormatFlags        = kMPEG4Object_AAC_Main;
      asbd_.mChannelsPerFrame   = channels_;
      asbd_.mBytesPerPacket     = 0;
      asbd_.mBytesPerFrame      = 0;
      asbd_.mFramesPerPacket    = 1024;
      asbd_.mBitsPerChannel     = 0;
      asbd_.mReserved           = 0;
      return false;
    } else if (format == ASU_FORMAT_AAC) {
      asbd_.mSampleRate         = sr_;
      asbd_.mFormatID           = kAudioFormatMPEG4AAC;
      asbd_.mFormatFlags        = kMPEG4Object_AAC_Main;
      asbd_.mChannelsPerFrame   = channels_;
      asbd_.mBytesPerPacket     = 0;
      asbd_.mBytesPerFrame      = 0;
      asbd_.mFramesPerPacket    = 1024;
      asbd_.mBitsPerChannel     = 0;
      asbd_.mReserved           = 0;
    }
    return true;
  }
}

AudioFormat_CoreAudio::AudioFormat_CoreAudio() {
  m_supportedFormatsForReading.push_back(ASU_FORMAT_WAV);
  m_supportedFormatsForReading.push_back(ASU_FORMAT_AIFF);
  m_supportedFormatsForReading.push_back(ASU_FORMAT_MP3);
  m_supportedFormatsForReading.push_back(ASU_FORMAT_AAC);
  m_supportedFormatsForReading.push_back(ASU_FORMAT_UNKNOWN);
  
  m_supportedFormatsForWriting.push_back(ASU_FORMAT_WAV);
  m_supportedFormatsForWriting.push_back(ASU_FORMAT_AIFF);
  m_supportedFormatsForWriting.push_back(ASU_FORMAT_MP3);
  m_supportedFormatsForWriting.push_back(ASU_FORMAT_AAC);
}

bool AudioFormat_CoreAudio::loadFile(const std::string& path,
    AudioBuffer& buffer,
    float& samplingRate,
    void** formatDetail_) {
  
  // Instantiate an extended audio file object.
  ExtAudioFileRef audioFileObject = 0;
  CFStringRef filePathString(CFStringCreateWithCString(kCFAllocatorDefault, path.c_str(), kCFStringEncodingUTF8));
  CFURLRef url(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, filePathString, kCFURLPOSIXPathStyle, false));
  CFRelease(filePathString);
  if (!url) 
      return false;

  ExtAudioFileOpenURL(url, &audioFileObject);
  CFRelease(url);
	AudioStreamBasicDescription fileFormat;
	UInt32 propSize = sizeof(fileFormat);
	memset(&fileFormat, 0, sizeof(AudioStreamBasicDescription)); 
  
  // get various properties including sampling rate
  // 
	OSStatus err = ExtAudioFileGetProperty(audioFileObject, kExtAudioFileProperty_FileDataFormat, &propSize, &fileFormat);  
  if (noErr != err) {
    return false;
  }
  
  samplingRate = fileFormat.mSampleRate; // SR... the channels are already in the buffer
  size_t numberOfChannels = fileFormat.mChannelsPerFrame; // ---CHANNELS---
  
  SInt64 numberOfFrames64 = 0; // ---FRAMES---
  propSize = sizeof(numberOfFrames64);
  err = ExtAudioFileGetProperty(audioFileObject, kExtAudioFileProperty_FileLengthFrames, &propSize, &numberOfFrames64);  
  if (noErr != err) {
    return false;
  }
  
  float** aacBuffer;
  
  if (fileFormat.mFormatID == ASU_FORMAT_AAC) {
    buffer.resize(numberOfChannels, numberOfFrames64 - 2110 - 824);
    aacBuffer = new float*[numberOfChannels];
    for (int i = 0; i < numberOfChannels; ++i) {
      aacBuffer[i] = new float[numberOfFrames64];
    }
  } else {
    // eventually resize the buffer of the inMemoryAudioFIle
    buffer.resize(numberOfChannels, numberOfFrames64);
  }
  
  // we get normalized -1 to 1 float out of the file
  AudioStreamBasicDescription clientDataFormat = fileFormat;
  clientDataFormat.mFormatID = kAudioFormatLinearPCM;
  clientDataFormat.mFormatFlags = kLinearPCMFormatFlagIsFloat;
  clientDataFormat.mBitsPerChannel = 8 * sizeof(Float32);
  clientDataFormat.mChannelsPerFrame = numberOfChannels;
  clientDataFormat.mFramesPerPacket = 1;
  clientDataFormat.mBytesPerPacket = sizeof(Float32);
  clientDataFormat.mBytesPerFrame = sizeof(Float32);
  clientDataFormat.mFormatFlags |= kAudioFormatFlagIsNonInterleaved;
  
  err = ExtAudioFileSetProperty(audioFileObject, kExtAudioFileProperty_ClientDataFormat, sizeof(AudioStreamBasicDescription), &clientDataFormat);
  
  if (noErr != err) {
    return false;
  }
  
  AudioBufferList* convertedData = reinterpret_cast<AudioBufferList*>(new Byte[offsetof (AudioBufferList, mBuffers) + ((numberOfChannels == 1 ? 0 : numberOfChannels) * sizeof(AudioBuffer))]);
  convertedData->mNumberBuffers = numberOfChannels;
  for ( int i=0; i<numberOfChannels; i++ ) {
    convertedData->mBuffers[i].mNumberChannels = 1;
    convertedData->mBuffers[i].mDataByteSize = numberOfFrames64 * sizeof(float);
    if (fileFormat.mFormatID == ASU_FORMAT_AAC) {
     convertedData->mBuffers[i].mData = (void*)aacBuffer[i];
    } else {
     convertedData->mBuffers[i].mData = (void*)buffer.getChannelData(i);
    }
  }

  if (fileFormat.mFormatID == ASU_FORMAT_AAC) {
    // Read from the file (or in-memory version)
    UInt32 framesToRead = numberOfFrames64;
    err = ExtAudioFileRead(audioFileObject, &framesToRead, convertedData);
    if (err != noErr) {
      for (int ch = 0; ch < numberOfChannels; ++ch) {
        delete [] aacBuffer[ch];
      }
      delete [] aacBuffer;
      delete convertedData;
      return false;
    }
    for (int ch = 0; ch < numberOfChannels; ++ch) {
      memcpy((void*)buffer.getChannelData(ch),
        (const void*)((float*)convertedData->mBuffers[ch].mData + 2110),
        sizeof(float) * (numberOfFrames64 - 2110 - 824));
    }
    for (int ch = 0; ch < numberOfChannels; ++ch) {
      delete [] aacBuffer[ch];
    }
    delete [] aacBuffer;
  } else {
    UInt32 framesToRead = numberOfFrames64;
    err = ExtAudioFileRead(audioFileObject, &framesToRead, convertedData);
    if (err != noErr) {
      delete convertedData;
      return false;
    }
  }
  delete convertedData;
  ExtAudioFileDispose(audioFileObject);
  // IMPORTANT: set the isReady flag to true
  buffer.isSilent = false;
  return true;
}

// http://stackoverflow.com/questions/12569015/writing-buffer-of-audio-samples-to-aac-file-using-extaudiofilewrite-for-ios

#define BUFFER_SIZE 4096


bool AudioFormat_CoreAudio::writeFile(const std::string& path,
    AudioBuffer& buffer,
    const float samplingRate,
    const AudioFormatTypes format_,
    const void* formatDetail_) {
    
  OSStatus err; // to record errors from ExtAudioFile API functions

  // create file path as CStringRef
  CFStringRef fPath;
  fPath = CFStringCreateWithCString(kCFAllocatorDefault,
                                    path.c_str(),
                                    kCFStringEncodingMacRoman);


  // specify total number of samples per channel
  //UInt32 totalFramesInFile = buffer.usedSize;




  /////////////////////////////////////////////////////////////////////////////
  ////////////////// Specify The Output Audio File Format /////////////////////
  /////////////////////////////////////////////////////////////////////////////


  // the client format will describe the output audio file
  AudioStreamBasicDescription clientFormat;

  // the file type identifier tells the ExtAudioFile API what kind of file we want created
  AudioFileTypeID fileType = 0;
  switch (format_) {
    case ASU_FORMAT_WAV:
      fileType = kAudioFileWAVEType;
      break;
    case ASU_FORMAT_AIFF:
      fileType = kAudioFileAIFFType;
      break;
    case ASU_FORMAT_MP3:
      fileType = kAudioFileMP3Type;
      break;
    case ASU_FORMAT_AAC:
      fileType = kAudioFileM4AType;
      break;
    default:
      assert(false && "fill here when adding new formats!");
      break;
  }

  fillASBD(clientFormat, format_, samplingRate, buffer.channels);


  /////////////////////////////////////////////////////////////////////////////
  ///////////////// Specify The Format of Our Audio Samples ///////////////////
  /////////////////////////////////////////////////////////////////////////////

  // the local format describes the format the samples we will give to the ExtAudioFile API
  AudioStreamBasicDescription localFormat;
  
  
  FillOutASBDForLPCM (localFormat,samplingRate,buffer.channels,32,32,true,false,false);


  /////////////////////////////////////////////////////////////////////////////
  ///////////////// Create the Audio File and Open It /////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  // create the audio file reference
  ExtAudioFileRef audiofileRef;

  // create a fileURL from our path
  CFURLRef fileURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,fPath,kCFURLPOSIXPathStyle,false);

  // open the file for writing
  err = ExtAudioFileCreateWithURL((CFURLRef)fileURL, fileType, &clientFormat, NULL, kAudioFileFlags_EraseFile, &audiofileRef);

  if (err != noErr)
  {
    std::cerr << "Problem when creating audio file: " << err << "\n";
    return false;
  }


  /////////////////////////////////////////////////////////////////////////////
  ///// Tell the ExtAudioFile API what format we'll be sending samples in /////
  /////////////////////////////////////////////////////////////////////////////

  // Tell the ExtAudioFile API what format we'll be sending samples in 
  err = ExtAudioFileSetProperty(audiofileRef, kExtAudioFileProperty_ClientDataFormat, sizeof(localFormat), &localFormat);

  if (err != noErr) {
    std::cerr << "Problem setting audio format: " << err << "\n";
    return false;
  }

  /////////////////////////////////////////////////////////////////////////////
  ///////// Write the Contents of the AudioBufferList to the AudioFile ////////
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  ////////////// Set up Audio Buffer List For Interleaved Audio ///////////////
  /////////////////////////////////////////////////////////////////////////////


  int channelCount = buffer.channels;

  AudioSampleType* tempBuffer = new AudioSampleType[BUFFER_SIZE * channelCount];
  AudioBufferList bufferList;
  bufferList.mNumberBuffers = 1;
  bufferList.mBuffers[0].mNumberChannels = buffer.channels;
  bufferList.mBuffers[0].mData = tempBuffer;
  bufferList.mBuffers[0].mDataByteSize = BUFFER_SIZE * sizeof (AudioUnitSampleType) * channelCount;
  
  // todo: interleaving sucks!
  // can use the accelerate framework ctoz for this!

  size_t count = 0;
  size_t running = 0;
  while(running < buffer.usedSize) {
    count = std::min((size_t)BUFFER_SIZE, (size_t)buffer.usedSize - running);
    for (int i = 0,  ii = 0; i < count; ++i, ++running) {
      for (int chan = 0; chan < buffer.channels; ++chan) {
        tempBuffer[ii++] = *(buffer.data[chan] + running);
      }
    }
    bufferList.mBuffers[0].mDataByteSize = count * sizeof (AudioUnitSampleType) * channelCount;
    err = ExtAudioFileWrite(audiofileRef, count, &bufferList);
    if (err != noErr) {
      std::cerr << "Problem writing audio file: " << err << "\n";
      ExtAudioFileDispose(audiofileRef);
      return false;
    }
  }

  delete tempBuffer;

  /////////////////////////////////////////////////////////////////////////////
  ////////////// Close the Audio File and Get Rid Of The Reference ////////////
  /////////////////////////////////////////////////////////////////////////////

  // close the file
  ExtAudioFileDispose(audiofileRef);


  return true;
}


}
}

