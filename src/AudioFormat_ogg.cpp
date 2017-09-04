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

 
#include "AudioFormat_ogg.hpp"
#include "stb_vorbis.c"

namespace asu {
namespace assets {

AudioFormat_ogg::AudioFormat_ogg() {
  m_supportedFormatsForReading.push_back(ASU_FORMAT_OGG);
}

bool AudioFormat_ogg::loadFile(const std::string& path,
  AudioBuffer& outBuf,
  float& samplingRate,
  void** formatDetail_) {
    try {
    short *decoded;
    int channels, len;
    len = stb_vorbis_decode_filename(const_cast<char*>(path.c_str()), &channels, &decoded);
    if (len <= 0) {
      return false;
    }
    outBuf.resize(channels, len);
    // deinterleave
    int frameIndex = 0;
    short* audioDataIn = NULL;
    float* audioDataOut = NULL;
    for (int chan = 0; chan < channels; ++chan) {
      audioDataIn = decoded + chan;
      audioDataOut = outBuf.data[chan];
      frameIndex = 0;
      while (frameIndex++ < len) {
        *audioDataOut = (float)*audioDataIn / 32767.F;
        audioDataIn += channels;
        ++audioDataOut;
      }
    }
    outBuf.isSilent = false;
    samplingRate = 44100; // todo: the stb_vorbis_decode_filename is the only one that doesnt show the samplingrate :/
    return true;
  } catch (...) {
    return false;
  }
}

bool AudioFormat_ogg::writeFile(const std::string& path,
  AudioBuffer& buffer,
  const float samplingRate,
  const AudioFormatTypes format_,
  const void* formatDetail_) {
  return false;
}

}
}
