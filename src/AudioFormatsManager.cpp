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

#include "AudioFormatsManager.hpp"
#include "StringUtilities.h"

#include <iostream>
#include "AudioFormat.hpp"

#ifdef ASUTILITIES_USE_COREAUDIO
#include "AudioFormat_CoreAudio.hpp"
#endif

#ifdef ASUTILITIES_USE_AAC
#include "AudioFormat_aac.hpp"
#endif

#ifdef ASUTILITIES_USE_OGG
#include "AudioFormat_ogg.hpp"
#endif

#ifdef ASUTILITIES_USE_SNDFILE
#include "AudioFormat_sndfile.hpp"
#endif

namespace asu {
namespace assets {
 
AudioFormatsManager::AudioFormatsManager() {
  #if defined(ASUTILITIES_USE_COREAUDIO)
  addFormat(std::shared_ptr<AudioFormat>(new AudioFormat_CoreAudio()));
  #endif

  #ifdef ASUTILITIES_USE_AAC
  addFormat(std::shared_ptr<AudioFormat>(new AudioFormat_aac()));
  #endif

  #ifdef ASUTILITIES_USE_OGG
  addFormat(std::shared_ptr<AudioFormat>(new AudioFormat_ogg()));
  #endif

  #ifdef ASUTILITIES_USE_SNDFILE
  addFormat(std::shared_ptr<AudioFormat>(new AudioFormat_sndfile()));
  #endif
}


void AudioFormatsManager::addFormat(std::shared_ptr<AudioFormat> fmt_) {
  for (auto format: fmt_->getSupportedFormatsForReading()) {
    if (m_formatsForReading.find(format) != m_formatsForReading.end()) {
      std::cerr << "The format " << formatToStr(format) << " has already been added to the AudioFormatManager (reading)" << std::endl;
      continue;
    }
    m_formatsForReading.insert(std::make_pair(format, fmt_));
  }
  for (auto format: fmt_->getSupportedFormatsForWriting()) {
    if (m_formatsForWriting.find(format) != m_formatsForWriting.end()) {
      std::cerr << "The format " << formatToStr(format) << " has already been added to the AudioFormatManager (writing)" << std::endl;
      continue;
    }
    m_formatsForWriting.insert(std::make_pair(format, fmt_));
  }
}

bool AudioFormatsManager::loadFile(const std::string& path_,
    AudioBuffer& buffer_,
    float& samplingRate_,
    void** formatDetail_) {
  std::string extension = utilities::getFileExtension(path_);
  auto formatForFile = m_formatsForReading.find(extensionToAudioFormat(extension.c_str()));
  if (formatForFile == m_formatsForReading.end()) {
    std::cerr << "No decoder for file " << path_ << std::endl;
    return false;
  }
  return formatForFile->second->loadFile(path_, buffer_, samplingRate_);
}

bool AudioFormatsManager::writeFile(const std::string& path_,
    AudioBuffer& buffer_,
    const float samplingRate_,
    const AudioFormatTypes format_,
    const void* formatDetail_) {
  std::string extension = utilities::getFileExtension(path_);
  auto formatForFile = m_formatsForWriting.find(format_);
  if (formatForFile == m_formatsForWriting.end()) {
    std::cerr << "No encoder for type " << formatToStr(format_) << std::endl;
    return false;
  }
  return formatForFile->second->writeFile(path_, buffer_, samplingRate_, format_);
}

bool AudioFormatsManager::getFileInfo(const std::string& path_,
  AudioFormatTypes& format_,
  float& samplingRate_,
  unsigned int& numberOfChannels_,
  unsigned int& bitsPerChannel_,
  unsigned long& length_) {
  
  std::string extension = utilities::getFileExtension(path_);
  auto formatForFile = m_formatsForReading.find(extensionToAudioFormat(extension.c_str()));
  if (formatForFile == m_formatsForReading.end()) {
    std::cerr << "No decoder for file " << path_ << std::endl;
    return false;
  }
  format_ = formatForFile->first;
  return formatForFile->second->getFileInfo(path_, samplingRate_, numberOfChannels_, bitsPerChannel_, length_);
}

}}
