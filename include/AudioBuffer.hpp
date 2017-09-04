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

 
/*!
 * \file AudioBuffer.hpp
 *
 * \author Alessandro Saccoia
 * \date 3/12/12
 * \updated 2/13/14
 *
 * \todo Make the sum method smarter...
 * \attention WIth the last updated I made some changes that can break legacy code using the sum and zero methods :/
 */


#ifndef __AUDIOBUFFER_HPP__
#define __AUDIOBUFFER_HPP__
#include <functional>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <random>

#ifdef USE_SAMPLERATE
#include "samplerate.h"
#endif

namespace asu {


template <class FTYPE>
struct AudioBufferC {
public:
  AudioBufferC() :
    channels(0),
    usedChannels(0),
    size(0),
    usedSize(0),
    data(NULL),
    isSilent(true),
    storage(NULL) {

  }

	AudioBufferC(size_t nchan_, size_t size_) :
    channels(0),
    usedChannels(0),
    size(0),
    usedSize(0),
    data(NULL),
    isSilent(true),
    storage(NULL) {
    resize(nchan_, size_);
  }

	~AudioBufferC() {
    if (usedChannels && size) {
      delete [] data;
      delete [] storage;
    }
  }
  
  // this method mixes and expands
  AudioBufferC& mix(const AudioBufferC& rhs) {
    if (!rhs.isSilent && rhs.usedChannels) {
      if (this->isSilent || !usedChannels) {
        this->resize(rhs.channels, rhs.size);
        *this = rhs;
        return *this;
      }
    }
    return *this;
  }

  AudioBufferC(const AudioBufferC& rhs, bool convertToMono = false) {
    data = nullptr;
    if (convertToMono && rhs.usedChannels != 1) {
      if (rhs.usedChannels == 0) {
        assert(false);
      } else {
        this->resize(1, rhs.size);
        for (int nChannel = 1; nChannel < rhs.usedChannels; ++nChannel) {
          std::transform(rhs.data[0],
            rhs.data[0] + usedSize,
            rhs.data[nChannel],
            data[0],
            std::plus<FTYPE>());
        }
        std::transform(data[0], data[0] + usedSize,
            data[0],
            std::bind2nd(std::multiplies<FTYPE>(), 1.0F / (FTYPE)usedChannels));
        usedChannels = 1;
      }
    } else {
      this->resize(rhs.channels, rhs.size);
      if (!rhs.isSilent) {
        for(int nChannel = 0; nChannel < rhs.usedChannels; ++nChannel) {
          FTYPE *pIn1 = data[nChannel];
          FTYPE *pIn2 = rhs.data[nChannel];
          std::copy(pIn2, pIn2 + size, pIn1);
        }
        this->isSilent = false;
      } else {
        this->isSilent = true;
      }
      usedChannels = rhs.usedChannels;
    }
  }

  void resize(size_t nchan_, size_t size_) {
    if (nchan_ == channels && size_ == size) {
      return;
    }
    usedChannels = channels = nchan_;
    size = size_;
    if (data != nullptr) {
      delete [] data;
      delete [] storage;
    }
    data = new FTYPE*[channels];
    storage = new FTYPE[size * channels];
    usedSize = size;
    for (unsigned int i = 0; i < channels; ++i)
      data[i] = storage + i * size_;
  }

  FTYPE* operator[] (const size_t channel) {
    return data[channel];
  }

  FTYPE* getChannelData (const size_t channel) const {
    assert(channel < usedChannels);
    return data[channel];
  }

  void zero(int count_ = -1, int position_ = 0) {
    if (count_ == -1) {
      isSilent = true;
      return;
    }
    assert(size >= position_ + count_);
    for (unsigned int i = 0; i < usedChannels; ++i)
      std::fill(data[i] + position_, data[i] + position_ + count_, FTYPE(0));
  }
  
  AudioBufferC& zeroAndcopy(const AudioBufferC& rhs) {
    zero();
    if (!usedChannels) {
      return *this;
    }
    if (!rhs.isSilent) {
      for(int nChannel = 0; nChannel < rhs.usedChannels; ++nChannel) {
        FTYPE *pIn1 = data[nChannel];
        FTYPE *pIn2 = rhs.data[nChannel];
        std::copy(pIn2, pIn2 + size, pIn1);
      }
      usedChannels = std::max(usedChannels, rhs.usedChannels);
      isSilent = false;
    }
    return *this;
  }
  
  AudioBufferC& operator=(const AudioBufferC& rhs) {
    this->resize(rhs.channels, rhs.size);
    if (!rhs.isSilent) {
      for(int nChannel = 0; nChannel < rhs.usedChannels; ++nChannel) {
        FTYPE *pIn1 = data[nChannel];
        FTYPE *pIn2 = rhs.data[nChannel];
        std::copy(pIn2, pIn2 + size, pIn1);
      }
      this->isSilent = false;
    } else {
      this->isSilent = true;
    }
    usedChannels = rhs.usedChannels;
    return *this;
  }
  
  template <class RHST>
  AudioBufferC& operator=(const AudioBufferC<RHST>& rhs) {
    this->resize(rhs.channels, rhs.size);
    if (!rhs.isSilent) {
      for(int nChannel = 0; nChannel < rhs.usedChannels; ++nChannel) {
        FTYPE *pIn1 = data[nChannel];
        RHST *pIn2 = rhs.data[nChannel];
        std::copy(pIn2, pIn2 + size, pIn1);
      }
      this->isSilent = false;
    } else {
      this->isSilent = true;
    }
    usedChannels = rhs.usedChannels;
    return *this;
  }
  
  // this handles just mono to stereo, and "same to same" for now
  AudioBufferC& sum(const AudioBufferC& rhs, int numSamples) {
    if (!usedChannels) {
      return *this;
    }
    if (!rhs.isSilent && rhs.usedChannels) {
      if (this->isSilent) {
        for (unsigned int i = 0; i < std::max(usedChannels, rhs.usedChannels); ++i)
          std::fill(data[i] , data[i] + size, FTYPE(0));
        isSilent = false;
      }
      // TODO generalize for ambisonics :)
      if (usedChannels == rhs.usedChannels) {
        for (int chan = 0; chan < std::max(usedChannels, rhs.usedChannels); ++chan) {
          float *pL1 = data[chan];
          float *pM2 = rhs.data[chan];
          std::transform(pL1, pL1 + numSamples, pM2, pL1, std::plus<FTYPE>());
        }
      } else if (usedChannels == 2 && rhs.usedChannels == 1) {
        float *pL1 = data[0];
        float *pR1 = data[1];
        float *pM2 = rhs.data[0];
        std::transform(pL1, pL1 + numSamples, pM2, pL1, std::plus<FTYPE>());
        std::transform(pR1, pR1 + numSamples, pM2, pR1, std::plus<FTYPE>());
      } else if (usedChannels == 1 && rhs.usedChannels == 2){
        float *pL1 = data[0];
        float *pR1 = data[1];
        float *pL2 = rhs.data[0];
        float *pR2 = rhs.data[1];
        std::transform(pL1, pL1 + numSamples, pR2, pR1, std::plus<FTYPE>());
        std::transform(pL1, pL1 + numSamples, pL2, pL1, std::plus<FTYPE>());
      } else {
        assert(false && "Summing is not supported for this channel configuration");
      }
      usedChannels = std::max(usedChannels, rhs.usedChannels);
      this->isSilent = false;
    }
    return *this;
  }
    // this handles just mono to stereo, and "same to same" for now
  AudioBufferC& sum(const AudioBufferC& rhs, int numSamples, float panning) {
    assert(false);
    // todo: this is a quick hack just for game of thrones
    assert(!rhs.isSilent && !this->isSilent && usedChannels == 2 && rhs.usedChannels == 1);

    if (usedSize < numSamples) {
      resize(2, numSamples, true);
    }
    PanPot panL(panning, 0);
    PanPot panR(panning, 1);
    float *pL1 = data[0];
    float *pR1 = data[1];
    float *pM2 = rhs.data[0];
    std::transform(pL1, pL1 + numSamples, pM2, pL1, panL);
    std::transform(pR1, pR1 + numSamples, pM2, pR1, panR);

    return *this;  
  }
  
  
  AudioBufferC& fill(const FTYPE value, const int numSamples, int position_ = 0) {  
    assert(usedChannels > 0);
    if (numSamples == 0) return *this;
    for (unsigned int i = 0; i < usedChannels; ++i)
      std::fill(data[i] + position_, data[i] + position_ + numSamples, value);
    return *this;
  }
  
  void createNoise(FTYPE min_, FTYPE max_) {    
    std::mt19937 generator;
    std::uniform_real_distribution<FTYPE> uniform_distribution(min_, max_);
    auto my_rand = std::bind(uniform_distribution, generator);
    for (unsigned int i = 0; i < usedChannels; ++i)
      std::generate(data[i], data[i] + usedSize, my_rand);
  }
  
  void addSine(FTYPE freq_, FTYPE sr_, FTYPE amp_ = 1.0) {
//    class MyOp {
//      MyOp(
//      FTYPE operator()() {
//        FTYPE toReturn = sin(phaseAcc);
//        phaseAcc += phaseInc;
//        return toReturn;
//      }
//      FTYPE phaseAcc;
//      FTYPE phaseInc;
//    };
//    MyOp a;
//    a.phaseInc =
    for (int i = 0; i < size; ++i) {
      for (int ch = 0; ch < usedChannels; ++ch) {
        data[ch][i] += amp_ * sin(2.0 * M_PI * freq_ * (FTYPE)i / sr_);
      }
    }
  }

  AudioBufferC& operator+= (const AudioBufferC& rhs) {
    assert(false);
  }
  
  #ifdef USE_SAMPLERATE
  
  void resample(float fromSr_, float toSr_) {
    SRC_DATA srcdata;
    srcdata.src_ratio = toSr_ / fromSr_;
    int newsize = ceil(size * srcdata.src_ratio);
    srcdata.input_frames = size;
    srcdata.output_frames = newsize;
    
    FTYPE** newdata = new FTYPE*[channels];
    FTYPE* newstorage = new FTYPE[newsize * channels];
    for (int i = 0; i < channels; ++i)
      newdata[i] = newstorage + i * newsize;
    
    for(int nChannel = 0; nChannel < channels; ++nChannel) {
      srcdata.data_in = data[nChannel];
      srcdata.data_out = newdata[nChannel];
      if (src_simple (&srcdata, SRC_SINC_MEDIUM_QUALITY, 1)) {
        exit(1);
      }
    }
    delete [] data;
    delete [] storage;
    data = newdata;
    storage = newstorage;
    usedSize = size = newsize;
    
  }
  
  #endif
  
  void setUsedChannels(size_t usedChannels_) {
    assert(usedChannels_ <= channels && 3 > usedChannels_);
    if (usedChannels < usedChannels_) {
      for (unsigned int i = usedChannels; i < usedChannels_; ++i)
        std::fill(data[i], data[i] + size, FTYPE(0));
    }
    usedChannels = usedChannels_;
  }
  
  void normalize() {
    if (usedChannels < 1) return;
    FTYPE maximum, max, min;
    maximum = 0;
    max=0;
    min=0;
    for(int nChannel = 0; nChannel < usedChannels; ++nChannel) {
      max = *std::max_element(data[nChannel],data[nChannel] + usedSize);
      min = *std::min_element(data[nChannel],data[nChannel] + usedSize);
      if (fabs(max)>fabs(min)) {
        maximum = std::max(maximum, (FTYPE)fabs(max));
      } else {
        maximum = std::max(maximum, (FTYPE)fabs(min));
      }
    }
    applyGain(FTYPE(1)/maximum);
  }
  
  void removeDC() {
    for(int nChannel = 0; nChannel < usedChannels; ++nChannel) {
      float avg = std::accumulate(data[nChannel], data[nChannel] + size, .0F) / size;
      std::transform(data[0], data[0] + usedSize,
        data[0],
        std::bind2nd(std::minus<FTYPE>(), avg));
    }
  }
  
  void applyGain(FTYPE linearGain) {
    for (int nChannel = 0; nChannel < usedChannels; ++nChannel) {
      std::transform(data[nChannel], data[nChannel] + usedSize,
        data[nChannel],
        std::bind2nd(std::multiplies<FTYPE>(), linearGain));
    }
  }
  
  void applyOnePole(FTYPE coefficient) {
    for (int nChannel = 0; nChannel < usedChannels; ++nChannel) {
      std::transform(data[nChannel]+1, data[nChannel] + usedSize,
        data[nChannel],
        data[nChannel],
        [&](const FTYPE& x, const FTYPE& x_1) { return x - coefficient * x_1; });
    }
  }
  
  void convertToMono() {
    if (usedChannels == 0) {
      assert(false);
    } else if (usedChannels == 1) {
      return;
    }
    for (int nChannel = 1; nChannel < usedChannels; ++nChannel) {
      std::transform(data[0],
        data[0] + usedSize,
        data[nChannel],
        data[0],
        std::plus<FTYPE>());
    }
    std::transform(data[0], data[0] + usedSize,
        data[0],
        std::bind2nd(std::multiplies<FTYPE>(), 1.0F / (FTYPE)usedChannels));
    usedChannels = 1;
  }
  
  AudioBufferC& operator=(const float value) {
    isSilent = true;
    channels = 1;
    return *this;
  }
  
  static inline void convolveTd(AudioBufferC& inputSignal, AudioBufferC& impulse, AudioBufferC& outputConv) {
    outputConv.resize(inputSignal.usedChannels, inputSignal.size + impulse.size - 1);
    for (unsigned int chan = 0; chan < inputSignal.usedChannels; ++chan) {
      int k = 0, o = 0, i = 0;
      float *inputData = inputSignal.data[chan];
      float *impulseData = impulse.data[impulse.usedChannels > chan ? chan : 0];
      float *outputData = outputConv.data[chan];
      for (o = 0; o < outputConv.size; ++o) {
        outputData[o] = .0F;
        for (k = 0; k < impulse.size; ++k) {
          i = o - k;
          if (i < 0)
            break;
          if (i > inputSignal.size) {
            continue;
          }
          outputData[o] += inputData[i] * impulseData[k];
        }
      }
    }
  }
  
  size_t channels;
  size_t usedChannels;
  size_t size;
  size_t usedSize;
  FTYPE** data;
  bool isSilent;
private:
  FTYPE* storage;
  
  
  /* MATLAB 
   scale = 2.0 - 4.0 * 10^(-3/20.0);
   degrees = [-90:1:90];
   degrees_norm = (degrees + 90) ./180;
   panL = 1 - degrees_norm;
   panR = degrees_norm;
   gainL = scale .* (panL .* panL) + (1.0 - scale) .* panL;
   gainR = scale .* (panR .* panR) + (1.0 - scale) .* panR;
  
  */
  #define SCALE_PAN_LAW_ATTENUATION_3DB (-0.8318F)
  struct PanPot : public std::binary_function<FTYPE,FTYPE,FTYPE> {
    // angle relative to the nose, channel 0 = L, 1 = R
    PanPot(float angle, int channel) {
      angle = (angle + 90.F) / 180.F;
      if (channel == 0) {
        angle = 1.F - angle;
      }
      gain = SCALE_PAN_LAW_ATTENUATION_3DB * (angle * angle) + (1.0F - SCALE_PAN_LAW_ATTENUATION_3DB) * angle;
    }
    //*result=binary_op(*first1,*first2++);
    inline FTYPE operator() (FTYPE a, FTYPE b) {
      return a + (b * gain);
    }
  private:
    FTYPE gain;
  };
};

typedef AudioBufferC<float> AudioBuffer;

} // dinahmoe


#endif // __AUDIOBUFFER_HPP__
