
#include "AudioFormat_sndfile.hpp"
#include <iostream>
#include "sndfile.h"

static bool sndfileCopyrightWritten = false;
void LibsndfileCopyright()
{
  if (sndfileCopyrightWritten) return;
  char vSndfileversion [256];
  sf_command (NULL, SFC_GET_LIB_VERSION, vSndfileversion, sizeof (vSndfileversion));
  std::cout << std::endl << "Using snd file IO: " << vSndfileversion << ", Copyright Erik de Castro Lopo, \nlicensed under the Gnu LGPL (see: http://www.mega-nerd.com/libsndfile or libsndfile.License)" << std::endl << std::endl;
  sndfileCopyrightWritten = true;
}


namespace dinahmoe {
namespace assets {

AudioFormat_sndfile::AudioFormat_sndfile() {
  LibsndfileCopyright();
  m_supportedFormatsForReading.push_back(DM_FORMAT_WAV);
  m_supportedFormatsForReading.push_back(DM_FORMAT_AIFF);
  m_supportedFormatsForWriting.push_back(DM_FORMAT_WAV);
  m_supportedFormatsForWriting.push_back(DM_FORMAT_AIFF);
}

#define BUFFER_SIZE 512

  // pass a ptr to AudioFormat if you wanna know the format of the decoded file
bool AudioFormat_sndfile::loadFile(const std::string& path,
  AudioBuffer& buffer,
  float& samplingRate,
  void** formatDetail_) {
  
  SNDFILE* infile;
  SF_INFO info;
  
  if (!(infile = sf_open(path.c_str(), SFM_READ, &info))) {
    std::cerr << "Not able to open input file " << path << std::endl;
		return false;
	} 
//  
//  if (formatOut_ != NULL) {
//    if ((info.format % SF_FORMAT_WAV) == SF_FORMAT_WAV) {
//      *formatOut_ = DM_FORMAT_WAV;
//    } else if ((info.format & SF_FORMAT_AIFF) == SF_FORMAT_AIFF) {
//      *formatOut_ = DM_FORMAT_AIFF;
//    } else {
//      assert(false);
//    }
//  } 
  
  samplingRate = info.samplerate;
	unsigned int v_readcount = 0;
  buffer.resize(info.channels, info.frames);
  
  if (info.channels == 1) {
    size_t count = 0;
    size_t running = 0;
    do {
      count = std::min((size_t)BUFFER_SIZE, (size_t)info.frames - running);
      if((v_readcount = sf_read_float(infile, buffer.data[0] + running, count)) != count) {
        std::cerr << "Error reading the input file!" << std::endl;
        return false;
      }
      running += v_readcount;
    } while (running < info.frames);
  } else {
    size_t count = 0;
    size_t running = 0;
    float buf[BUFFER_SIZE * 2];
    do {
      count = std::min((size_t)BUFFER_SIZE, (size_t)info.frames - running) * 2;
      if((v_readcount = sf_read_float(infile, buf, count)) != count) {
        std::cerr << "Error reading the input file!" << std::endl;
        return false;
      }
      int i = 0;
      while(i < v_readcount) {
        *(buffer.data[0] + running) = buf[i++];
        *(buffer.data[1] + running) = buf[i++];
        ++running;
      }
    } while (running < info.frames);
  }
  
  buffer.isSilent = false;
  
  sf_close(infile);
  return true;
}

// the format specifies the type, not the extension!
bool AudioFormat_sndfile::writeFile(const std::string& path,
  AudioBuffer& buffer,
  const float samplingRate,
  const AudioFormatTypes format_,
  const void* formatDetail_) {  
  SNDFILE* outfile;
  SF_INFO info;
  info.sections = 1;
  info.seekable = 1;
  info.samplerate = samplingRate;
  info.channels = buffer.channels;
  info.frames = buffer.size;
  info.format = 0;
  if (format_ == DM_FORMAT_WAV) {
    info.format = SF_FORMAT_WAV;
  } else if (format_ == DM_FORMAT_AIFF) {
    info.format = SF_FORMAT_AIFF;
  }
  info.format = info.format | SF_FORMAT_PCM_16;
  
  if (!(outfile = sf_open(path.c_str(), SFM_WRITE, &info))) {
    std::cerr << "Unable to open the output file " << path << std::endl;
    std::cerr << sf_strerror(outfile);
		return false;
	} 
  
	unsigned int v_writecount = 0;
  
  if (info.channels == 1) {
    if((v_writecount = sf_write_float(outfile, buffer.getChannelData(0), buffer.usedSize)) != buffer.usedSize) {
      std::cerr << "Error writing the output file!" << std::endl;
      return false; 
    }
  } else {
    size_t count = 0;
    size_t running = 0;
    float buf[BUFFER_SIZE * 2];
    while(running < buffer.size) {
      count = std::min((size_t)BUFFER_SIZE, (size_t)buffer.size - running);
      for (int i = 0,  ii = 0; i < count; ++i, ++running) {
        buf[ii++] = *(buffer.data[0] + running);
        buf[ii++] = *(buffer.data[1] + running);
      }
      if((v_writecount = sf_write_float(outfile, buf, count * 2)) != count * 2) {
        std::cerr << "Error writing the output file!" << std::endl;
        exit(1);  
      }
    }
  }
  sf_close(outfile);
  return true;
}


}
}