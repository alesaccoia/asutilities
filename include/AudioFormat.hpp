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

//  Created by Alessandro Saccoia on 1/19/15.

#ifndef Dmaf_OSC_Utilities_AudioFormat_hpp
#define Dmaf_OSC_Utilities_AudioFormat_hpp

#include <vector>
#include "AudioBuffer.hpp"
#include "AudioFormatTypes.h"

namespace asu {
namespace assets {

class AudioFormat {
public:
  // pass a ptr to AudioFormat if you wanna know the format of the decoded file
  virtual bool loadFile(const std::string& path,
    AudioBuffer& buffer,
    float& samplingRate,
    void** formatDetail_ = NULL) = 0;

  // the format specifies the type, not the extension!
  virtual bool writeFile(const std::string& path,
    AudioBuffer& buffer,
    const float samplingRate,
    const AudioFormatTypes format_,
    const void* formatDetail_ = nullptr) = 0;

  std::vector<AudioFormatTypes>& getSupportedFormatsForReading() { return m_supportedFormatsForReading; }
  std::vector<AudioFormatTypes>& getSupportedFormatsForWriting() { return m_supportedFormatsForWriting; }
protected:
  std::vector<AudioFormatTypes> m_supportedFormatsForReading;
  std::vector<AudioFormatTypes> m_supportedFormatsForWriting;
};

}
}


#endif
