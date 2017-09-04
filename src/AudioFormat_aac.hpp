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

//  Created by Alessandro Saccoia on 1/19/15.

#ifndef __AudioFormat_aac_
#define __AudioFormat_aac_

#include "AudioFormat.hpp"
#include <list>

namespace dinahmoe {
namespace assets {

class AudioFormat_aac : public AudioFormat {
public:
  AudioFormat_aac();
  ~AudioFormat_aac();

  // pass a ptr to AudioFormat if you wanna know the format of the decoded file
  bool loadFile(const std::string& path,
    AudioBuffer& buffer,
    float& samplingRate,
    void** formatDetail_ = NULL);

  // the format specifies the type, not the extension!
  bool writeFile(const std::string& path,
    AudioBuffer& buffer,
    const float samplingRate,
    const AudioFormatTypes format_,
    const void* formatDetail_ = nullptr);
  
  // this list contains the decoded buffers and it's reused
  typedef struct DecodedBuffer_ {
    DecodedBuffer_(signed short* ptr_,
      signed int usedFrames_,
      signed int allocatedSamples_) :
      ptr(ptr_), usedFrames(usedFrames_), allocatedSamples(allocatedSamples_) {}
    signed short* ptr;
    signed int usedFrames;
    signed int allocatedSamples;
  } DecodedBuffer;
  std::list<DecodedBuffer> m_buffers;
};

}
}

#endif /* defined(__AudioFormat_aac_) */
