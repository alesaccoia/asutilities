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

//  Created by Alessandro Saccoia on 11/26/13.

#ifndef dm_audiomidi_audioformats_h
#define dm_audiomidi_audioformats_h

#include <string.h>

namespace asu {
namespace assets {


/**@todo Create compile time definitions for the supported formats, and a C++11 style map with the names
*/

enum AudioFormatTypes {
  ASU_FORMAT_RAW,
  ASU_FORMAT_WAV,
  ASU_FORMAT_AIFF,
  ASU_FORMAT_OGG,
  ASU_FORMAT_MP3,
  ASU_FORMAT_AAC,
  ASU_FORMAT_UNKNOWN
};



inline AudioFormatTypes strToAudioFormat(const char* str_) {
  if (strcmp(str_, "ASU_FORMAT_RAW") == 0) {
    return ASU_FORMAT_RAW;
  } else if (strcmp(str_, "ASU_FORMAT_WAV") == 0) {
    return ASU_FORMAT_WAV;
  } else if (strcmp(str_, "ASU_FORMAT_AIFF") == 0) {
    return ASU_FORMAT_AIFF;
  } else if (strcmp(str_, "ASU_FORMAT_OGG") == 0) {
    return ASU_FORMAT_OGG;
  } else if (strcmp(str_, "ASU_FORMAT_MP3") == 0) {
    return ASU_FORMAT_MP3;
  } else if (strcmp(str_, "ASU_FORMAT_AAC") == 0) {
    return ASU_FORMAT_AAC;
  }
  else {
    return ASU_FORMAT_UNKNOWN;
  }
}

inline const char* formatToStr(const AudioFormatTypes& format_) {
  switch (format_) {
    case ASU_FORMAT_RAW:
      return "ASU_FORMAT_RAW";
      break;
    case ASU_FORMAT_WAV:
      return "ASU_FORMAT_WAV";
      break;
    case ASU_FORMAT_AIFF:
      return "ASU_FORMAT_AIFF";
      break;
    case ASU_FORMAT_OGG:
      return "ASU_FORMAT_OGG";
      break;
    case ASU_FORMAT_MP3:
      return "ASU_FORMAT_MP3";
      break;
    case ASU_FORMAT_AAC:
      return "ASU_FORMAT_AAC";
      break;
    default:
      return "ASU_FORMAT_UNKNOWN";
      break;
  }
}

inline AudioFormatTypes extensionToAudioFormat(const char* extension_) {
  std::string extension(extension_);
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
  if (extension == "raw") {
    return ASU_FORMAT_RAW;
  } else if (extension == "wav") {
    return ASU_FORMAT_WAV;
  } else if (extension == "aif") {
    return ASU_FORMAT_AIFF;
  } else if (extension == "aiff") {
    return ASU_FORMAT_AIFF;
  } else if (extension == "ogg") {
    return ASU_FORMAT_OGG;
  } else if (extension == "mp3") {
    return ASU_FORMAT_MP3;
  } else if (extension == "aac") {
    return ASU_FORMAT_AAC;
  } else {
    return ASU_FORMAT_UNKNOWN;
  }
}

}
}

#endif // dm_audiomidi_audioformats_h


