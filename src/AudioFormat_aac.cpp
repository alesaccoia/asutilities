/*
 * Copyright (c) 2015, Dinahmoe. All rights reserved.
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

 
/**
 * @date 1/20/2015
 * @author Alessandro Saccoia <alessandro@dinahmoe.com>
 * 
 * I could find no implementations of this. The relevant documents I've used:
 * https://github.com/mstorsjo/fdk-aac (see decoder-example)
 * aacDecoder.pdf as found in the fdk-aac download
 * http://wiki.multimedia.cx/index.php?title=MPEG-4_Audio
 * 
 */

#include "AudioFormat_aac.hpp"
#include <iostream>
#include <list>
#include "libAACenc/include/aacenc_lib.h"
#include "libAACdec/include/aacdecoder_lib.h"
#include "libMpegTPDec/include/mpegFileRead.h"
#include "libSYS/include/wav_file.h"

static bool aacCopyrightWritten = false;

void FDKAacCopyright()
{
  if (aacCopyrightWritten) return;
  std::cout << std::endl <<
    "Using AAC file IO: " << std::endl <<
    "The Fraunhofer FDK AAC Codec Library for Android" << std::endl <<
    "© Copyright  1995 - 2012 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V." << std::endl <<
    "All rights reserved."
    << std::endl << std::endl;
  aacCopyrightWritten = true;
}



namespace dinahmoe {
namespace assets {

AudioFormat_aac::AudioFormat_aac() {
  FDKAacCopyright();
  m_supportedFormatsForReading.push_back(DM_FORMAT_AAC);
  m_supportedFormatsForWriting.push_back(DM_FORMAT_AAC);
}

AudioFormat_aac::~AudioFormat_aac() {
  for (auto buf: m_buffers) {
    delete [] buf.ptr;
  }
}

/*
 Notes about the implementation
 Seems we are able to guess the exact number of channels just after the first time we get


*/

#define BUFFER_IN_SIZE 1024
#define BUFFER_OUT_SIZE 20480
#define AAC_EMPTY_SAMPLES_START 2110
#define AAC_EMPTY_SAMPLES_END 824
#define AAC_EMPTY_SAMPLES_TOTAL (AAC_EMPTY_SAMPLES_START + AAC_EMPTY_SAMPLES_END)

  // pass a ptr to AudioFormat if you wanna know the format of the decoded file
bool AudioFormat_aac::loadFile(const std::string& path_,
  AudioBuffer& buffer_,
  float& samplingRate_,
  void** formatDetail_) {
  HANDLE_AACDECODER handle;
  int channels;
  handle = aacDecoder_Open(TT_MP4_ADTS,1);
  FILE* aacFile = fopen(path_.c_str() ,"rb");
  if (aacFile == NULL) {
    std::cerr << "Problems opening file " << path_ << std::endl;
    return false;
  }
  UCHAR *inBuffer = (UCHAR*)malloc(2048);
  int readLen = -1;
  UINT bytesValid = 0;
  INT_PCM outBuffer[20480];
  int firstFrame = 1;
  int total = 0;
  int numberOfInputFrames = 0;
  std::list<DecodedBuffer>::iterator lastBufferIt = m_buffers.begin();
  INT_PCM* writingBuf;
  while(readLen != 0)
  {
    if (bytesValid != 2048) {
      readLen = fread((void*)(inBuffer + bytesValid) , 1, 2048 - bytesValid ,aacFile);
    }
    bytesValid = readLen;
    total += readLen;
    aacDecoder_Fill(handle, &inBuffer, (const UINT*)&readLen, &bytesValid);
    AAC_DECODER_ERROR errStatus;
    while ((errStatus = aacDecoder_DecodeFrame(handle, outBuffer, 20480 ,0)) != AAC_DEC_NOT_ENOUGH_BITS) {
      CStreamInfo* info = aacDecoder_GetStreamInfo(handle);
      if (firstFrame == 1) {
        firstFrame = 0;
        samplingRate_ = info->aacSampleRate;
        channels = info->numChannels;
      }
      // reuse the buffer on the back() of the list. there are 3 possibilities, in order:
      // - we need to allocate buffers in the list because there are not enough (lastBufferIt is at the end of the list)
      // - the buffer pointed by lastBufferIt can accomodate enough samples
      // - the buffer pointed by lastBufferIt can't accomodate enough samples: we delete and realloc
      // note that writingBuf at the end of the branch will point to the buffer after the last one used
      // that is end() or before.
      if (lastBufferIt == m_buffers.end()) {
        writingBuf = new INT_PCM[info->frameSize * channels];
        m_buffers.emplace_back(writingBuf, info->frameSize, info->frameSize * channels);
        lastBufferIt = m_buffers.end();
      } else {
        if (lastBufferIt->allocatedSamples >= info->frameSize * channels) {
          writingBuf = lastBufferIt->ptr;
          lastBufferIt->usedFrames = info->frameSize;
        } else {
          delete [] lastBufferIt->ptr;
          writingBuf = new INT_PCM[info->frameSize * channels];
          lastBufferIt->usedFrames = lastBufferIt->allocatedSamples = info->frameSize * channels;
        }
        lastBufferIt++;
      }
      memcpy((void*)writingBuf, (void*)outBuffer, info->frameSize * sizeof(INT_PCM) * channels);
      numberOfInputFrames += info->frameSize;
    }

    if (errStatus != AAC_DEC_OK) {
      if (errStatus == AAC_DEC_NOT_ENOUGH_BITS) {
        continue;
      }
      break;
    }
  }
  fclose(aacFile);
  // now setup the audiobuffer
  long numberOfOutputFrames = numberOfInputFrames - AAC_EMPTY_SAMPLES_TOTAL;
  long totalOutputSamples = numberOfOutputFrames * channels;
  buffer_.resize(channels, numberOfOutputFrames); // 2934
  std::list<DecodedBuffer>::iterator bufit = m_buffers.begin();
  long currentOutputSample = 0;
  
  long inputRunningSum = 0;
  long totalRunningSum = 0;
  long inputSampleToStartAt = AAC_EMPTY_SAMPLES_START * channels;
  float* outputStoragePtr = buffer_.getChannelData(0);
  int samplesInThisBuffer = 0;
  INT_PCM* inputPtr;
  while (bufit != lastBufferIt) {
    samplesInThisBuffer = bufit->usedFrames * channels;
    inputPtr = bufit->ptr;
    for (int i = 0; i < samplesInThisBuffer; ++i) {
      if (++inputRunningSum < inputSampleToStartAt) {
        continue;
      }
      if (++totalRunningSum >= totalOutputSamples) {
        break;
      }
      *(outputStoragePtr + currentOutputSample) = ((float)(*(inputPtr + i))) / 32767.F;
      currentOutputSample += numberOfOutputFrames;
      if (currentOutputSample >= totalOutputSamples) {
        currentOutputSample %= totalOutputSamples;
        ++currentOutputSample;
      }
    }
    ++bufit;
  }
  aacDecoder_Close(handle);
  return true;
}

// the format specifies the type, not the extension!
bool AudioFormat_aac::writeFile(const std::string& path,
  AudioBuffer& buffer,
  const float samplingRate,
  const AudioFormatTypes format_,
  const void* formatDetail_) {
  int bitrate = 64000;
  const char* outfile;
	FILE *out;
	int input_size;
	int16_t* convert_buf;
	int aot = 2;
	int afterburner = 1;
	int eld_sbr = 0;
	int vbr = 0;
	HANDLE_AACENCODER handle;
	CHANNEL_MODE mode;
	AACENC_InfoStruct info = { 0 };
//	while ((ch = getopt(argc, argv, "r:t:a:s:v:")) != -1) {
//		switch (ch) {
//		case 'r':
//			bitrate = atoi(optarg);
//			break;
//		case 't':
//			aot = atoi(optarg);
//			break;
//		case 'a':
//			afterburner = atoi(optarg);
//			break;
//		case 's':
//			eld_sbr = atoi(optarg);
//			break;
//		case 'v':
//			vbr = atoi(optarg);
//			break;
//		case '?':
//		default:
//			usage(argv[0]);
//			return 1;
//		}
//	}
//	if (argc - optind < 2) {
//		usage(argv[0]);
//		return 1;
//	}
	outfile = path.c_str();

	switch (buffer.channels) {
	case 1: mode = MODE_1;       break;
	case 2: mode = MODE_2;       break;
	case 3: mode = MODE_1_2;     break;
	case 4: mode = MODE_1_2_1;   break;
	case 5: mode = MODE_1_2_2;   break;
	case 6: mode = MODE_1_2_2_1; break;
	default:
		fprintf(stderr, "Unsupported WAV channels %zu\n", buffer.channels);
		return 1;
	}
	if (aacEncOpen(&handle, 0, buffer.channels) != AACENC_OK) {
		fprintf(stderr, "Unable to open encoder\n");
		return 1;
	}
	if (aacEncoder_SetParam(handle, AACENC_AOT, aot) != AACENC_OK) {
		fprintf(stderr, "Unable to set the AOT\n");
		return 1;
	}
	if (aot == 39 && eld_sbr) {
		if (aacEncoder_SetParam(handle, AACENC_SBR_MODE, 1) != AACENC_OK) {
			fprintf(stderr, "Unable to set SBR mode for ELD\n");
			return 1;
		}
	}
	if (aacEncoder_SetParam(handle, AACENC_SAMPLERATE, samplingRate) != AACENC_OK) {
		fprintf(stderr, "Unable to set the AOT\n");
		return 1;
	}
	if (aacEncoder_SetParam(handle, AACENC_CHANNELMODE, mode) != AACENC_OK) {
		fprintf(stderr, "Unable to set the channel mode\n");
		return 1;
	}
	if (aacEncoder_SetParam(handle, AACENC_CHANNELORDER, 1) != AACENC_OK) {
		fprintf(stderr, "Unable to set the wav channel order\n");
		return 1;
	}
	if (vbr) {
		if (aacEncoder_SetParam(handle, AACENC_BITRATEMODE, vbr) != AACENC_OK) {
			fprintf(stderr, "Unable to set the VBR bitrate mode\n");
			return 1;
		}
	} else {
		if (aacEncoder_SetParam(handle, AACENC_BITRATE, bitrate) != AACENC_OK) {
			fprintf(stderr, "Unable to set the bitrate\n");
			return 1;
		}
	}
	if (aacEncoder_SetParam(handle, AACENC_TRANSMUX, 2) != AACENC_OK) {
		fprintf(stderr, "Unable to set the ADTS transmux\n");
		return 1;
	}
	if (aacEncoder_SetParam(handle, AACENC_AFTERBURNER, afterburner) != AACENC_OK) {
		fprintf(stderr, "Unable to set the afterburner mode\n");
		return 1;
	}
	if (aacEncEncode(handle, NULL, NULL, NULL, NULL) != AACENC_OK) {
		fprintf(stderr, "Unable to initialize the encoder\n");
		return 1;
	}
	if (aacEncInfo(handle, &info) != AACENC_OK) {
		fprintf(stderr, "Unable to get the encoder info\n");
		return 1;
	}

	out = fopen(outfile, "wb");
	if (!out) {
		perror(outfile);
		return 1;
	}

	input_size = buffer.channels*2*info.frameLength;
	convert_buf = (int16_t*) malloc(input_size);
  
  int currentFrame = 0;
  int stride = buffer.size + 1;
  int16_t currentConvertedSample;
  long currentSampleIndex = 0;
  float * bufferStart = buffer.data[0];
  int sizeMod = buffer.size * buffer.channels + 1;
  uint8_t* sampleL;
	while (1) {
		AACENC_BufDesc in_buf = { 0 }, out_buf = { 0 };
		AACENC_InArgs in_args = { 0 };
		AACENC_OutArgs out_args = { 0 };
		int in_identifier = IN_AUDIO_DATA;
		int in_size, in_elem_size;
		int out_identifier = OUT_BITSTREAM_DATA;
		int out_size, out_elem_size;
		void *in_ptr, *out_ptr;
		uint8_t outbuf[20480];
		AACENC_ERROR err;
    int samplesToBeWritten = std::min((int)info.frameLength, (int)buffer.size - currentFrame - 1);
    samplesToBeWritten *= buffer.channels;
    int samplesThisFrame = 0;
    while (samplesThisFrame < samplesToBeWritten) {
      float fValue = *(bufferStart + currentSampleIndex);
      currentConvertedSample = (int16_t)(fValue * 32767.F);
      sampleL = (uint8_t*)&currentConvertedSample;
      convert_buf[samplesThisFrame] = sampleL[0] | (sampleL[1] << 8);
      currentSampleIndex = currentSampleIndex + stride;
      currentSampleIndex = currentSampleIndex % sizeMod;
      ++samplesThisFrame;
    }
    currentFrame  = currentFrame + (samplesThisFrame / buffer.channels);
		if (samplesThisFrame <= 0) {
			in_args.numInSamples = -1;
		} else {
			in_ptr = convert_buf;
			in_size = samplesThisFrame * 2;
			in_elem_size = 2;

			in_args.numInSamples = samplesThisFrame;
			in_buf.numBufs = 1;
			in_buf.bufs = &in_ptr;
			in_buf.bufferIdentifiers = &in_identifier;
			in_buf.bufSizes = &in_size;
			in_buf.bufElSizes = &in_elem_size;
		}
		out_ptr = outbuf;
		out_size = sizeof(outbuf);
		out_elem_size = 1;
		out_buf.numBufs = 1;
		out_buf.bufs = &out_ptr;
		out_buf.bufferIdentifiers = &out_identifier;
		out_buf.bufSizes = &out_size;
		out_buf.bufElSizes = &out_elem_size;

		if ((err = aacEncEncode(handle, &in_buf, &out_buf, &in_args, &out_args)) != AACENC_OK) {
			if (err == AACENC_ENCODE_EOF)
				break;
			fprintf(stderr, "Encoding failed\n");
			return false;
		}
		if (out_args.numOutBytes == 0)
			continue;
		fwrite(outbuf, 1, out_args.numOutBytes, out);
	}
	free(convert_buf);
	fclose(out);
	aacEncClose(&handle);
	return true;
}


}
}