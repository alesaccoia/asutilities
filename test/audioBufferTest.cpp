
#include <iostream>
#include <chrono>
#include "AudioBuffer.hpp"

using namespace dinahmoe;

int main (int argc, char** argv) {

  #pragma mark Test default ctor
  {
    AudioBuffer buf1;
    assert(buf1.channels == 0);
    assert(buf1.usedChannels == 0);
    assert(buf1.size == 0);
    assert(buf1.data == NULL);
  }
  #pragma mark test extended ctor and noise
  {
    AudioBuffer buf1(1,1000000);
    assert(buf1.channels == 1);
    assert(buf1.usedChannels == 1);
    assert(buf1.size == 1000000);
    assert(buf1.channelIsConstant[0]);
   
    buf1.resize(2, 1000);
    assert(buf1.channels == 2);
    assert(buf1.usedChannels == 2);
    assert(buf1.size == 1000);
    assert(buf1.channelIsConstant[0]);
    assert(buf1.channelIsConstant[1]);
    
    buf1.createNoise(-1.0F, 1.0F);
    assert(!buf1.channelIsConstant[0]);
    assert(!buf1.channelIsConstant[2]);
    // check if createNoise produces values within the expected range
    assert(std::find_if(buf1.getChannelData(0), buf1.getChannelData(0) + 1000,
      [](const float& val_) {
        if (fabs(val_) > 1.0F) {
          return true;
        } else {
          return false;
        }
      } ) == (buf1.getChannelData(0) + 1000));
  }
  
  #pragma mark test inplace arithmetics between constant values
  {
    AudioBuffer buf1(1,1000000);
    AudioBuffer buf2(1,1000000);
    for (size_t i = 1; i < 100; ++i) {
      size_t numberOfChannels1 = i;
      size_t numberOfChannels2 = i > 50 ? 1 : i;
      buf1.resize(numberOfChannels1, 1000000);
      buf2.resize(numberOfChannels2, 1000000);
      buf1.setIsConstant(true, 2.0F);
      buf2.setIsConstant(true, 1.0F);
      
      buf1.transform<std::plus>(buf2, 1000000);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == 1000000);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == 1000000);
      for (int ch = 0; ch < i; ++ch) {
        assert(buf1.channelIsConstant[ch]);
        assert(buf1.channelConstantValue[ch] == 3.0F);
      }
      
      buf1.setIsConstant(true, 2.0F);
      buf1.transform<std::minus>(buf2, 1000000);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == 1000000);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == 1000000);
      for (int ch = 0; ch < i; ++ch) {
        assert(buf1.channelIsConstant[ch]);
        assert(buf1.channelConstantValue[ch] == 1.0F);
      }
      
      buf1.setIsConstant(true, 2.0F);
      buf1.transform<std::multiplies>(buf2, 1000000);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == 1000000);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == 1000000);
      for (int ch = 0; ch < i; ++ch) {
        assert(buf1.channelIsConstant[ch]);
        assert(buf1.channelConstantValue[ch] == 2.0F);
      }
      
      buf1.setIsConstant(true, 2.0F);
      buf1.transform<std::divides>(buf2, 1000000);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == 1000000);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == 1000000);
      for (int ch = 0; ch < i; ++ch) {
        assert(buf1.channelIsConstant[ch]);
        assert(buf1.channelConstantValue[ch] == 2.0F);
      }
    }
  }
  
  #pragma mark test not inplace arithmetics between constant values
  {
    AudioBuffer buf1(1,1000000);
    AudioBuffer buf2(1,1000000);
    AudioBuffer bufOut(1,1000000);
    for (size_t i = 1; i < 10; ++i) {
      size_t numberOfChannels1 = i > 3 ? ((i <= 5) ? 1 : i) : i;
      size_t numberOfChannels2 = i > 5 ? 1 : i;
      size_t numberOfChannelsOut = std::max(numberOfChannels1, numberOfChannels2);
      buf1.resize(numberOfChannels1, 1000000);
      buf2.resize(numberOfChannels2, 1000000);
      bufOut.resize(numberOfChannelsOut, 1000000);
      buf1.setIsConstant(true, 2.0F);
      buf2.setIsConstant(true, 1.0F);
      
      AudioBuffer::transform<std::plus>(buf1,buf2,bufOut,1000000);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == 1000000);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == 1000000);
      for (int ch = 0; ch < i; ++ch) {
        assert(bufOut.channelIsConstant[ch]);
        assert(bufOut.channelConstantValue[ch] == 3.0F);
      }
      
      AudioBuffer::transform<std::minus>(buf1,buf2,bufOut,1000000);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == 1000000);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == 1000000);
      for (int ch = 0; ch < i; ++ch) {
        assert(bufOut.channelIsConstant[ch]);
        assert(bufOut.channelConstantValue[ch] == 1.0F);
      }
      
      AudioBuffer::transform<std::multiplies>(buf1,buf2,bufOut,1000000);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == 1000000);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == 1000000);
      for (int ch = 0; ch < i; ++ch) {
        assert(bufOut.channelIsConstant[ch]);
        assert(bufOut.channelConstantValue[ch] == 2.0F);
      }
      
      AudioBuffer::transform<std::divides>(buf1,buf2,bufOut,1000000);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == 1000000);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == 1000000);
      for (int ch = 0; ch < i; ++ch) {
        assert(bufOut.channelIsConstant[ch]);
        assert(bufOut.channelConstantValue[ch] == 2.0F);
      }
    }
  }
  
  #pragma mark test inplace arithmetics between non constant and constant buffer
  {
    const size_t numSamples = 1000000;
    AudioBuffer buf1;
    AudioBuffer buf2;
    for (size_t i = 1; i < 10; ++i) {
      size_t numberOfChannels1 = i;
      size_t numberOfChannels2 = i > 5 ? 1 : i;
      size_t numberOfChannelsOut = std::max(numberOfChannels1, numberOfChannels2);
      buf1.resize(numberOfChannels1, numSamples);
      buf1.fill(2.0F, numSamples);
      buf1.setIsConstant(false);
      buf2.resize(numberOfChannels2, numSamples);
      buf2.setIsConstant(true, 1.0F);
      
      buf1.transform<std::plus>(buf2,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!buf1.channelIsConstant[ch]);
        auto fbegin = buf1.getChannelData(ch);
        auto fend = buf1.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),3.0F)) == fend);
      }
      
      buf1.fill(2.0F, numSamples);
      buf1.setIsConstant(false);
      buf1.transform<std::minus>(buf2,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!buf1.channelIsConstant[ch]);
        auto fbegin = buf1.getChannelData(ch);
        auto fend = buf1.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),1.0F)) == fend);
      }
      
      buf1.fill(2.0F, numSamples);
      buf1.setIsConstant(false);
      buf1.transform<std::multiplies>(buf2,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!buf1.channelIsConstant[ch]);
        auto fbegin = buf1.getChannelData(ch);
        auto fend = buf1.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),2.0F)) == fend);
      }
      
      buf1.fill(2.0F, numSamples);
      buf1.setIsConstant(false);
      buf1.transform<std::divides>(buf2,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!buf1.channelIsConstant[ch]);
        auto fbegin = buf1.getChannelData(ch);
        auto fend = buf1.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),2.0F)) == fend);
      }
    }
  }
  
  #pragma mark test not inplace arithmetics between non constant and constant buffer
  {
    const size_t numSamples = 1000000;
    AudioBuffer buf1;
    AudioBuffer buf2;
    AudioBuffer bufOut;
    for (size_t i = 1; i < 10; ++i) {
      size_t numberOfChannels1 = i > 3 ? ((i <= 5) ? 1 : i) : i;
      size_t numberOfChannels2 = i > 5 ? 1 : i;
      size_t numberOfChannelsOut = std::max(numberOfChannels1, numberOfChannels2);
      buf1.resize(numberOfChannels1, numSamples);
      buf1.fill(2.0F, numSamples);
      buf1.setIsConstant(false);
      buf2.resize(numberOfChannels2, numSamples);
      buf2.setIsConstant(true, 1.0F);
      bufOut.resize(numberOfChannelsOut, numSamples);
      
      AudioBuffer::transform<std::plus>(buf1,buf2,bufOut,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!bufOut.channelIsConstant[ch]);
        auto fbegin = bufOut.getChannelData(ch);
        auto fend = bufOut.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),3.0F)) == fend);
      }
      
      AudioBuffer::transform<std::minus>(buf1,buf2,bufOut,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!bufOut.channelIsConstant[ch]);
        auto fbegin = bufOut.getChannelData(ch);
        auto fend = bufOut.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),1.0F)) == fend);
      }
      
      AudioBuffer::transform<std::multiplies>(buf1,buf2,bufOut,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!bufOut.channelIsConstant[ch]);
        auto fbegin = bufOut.getChannelData(ch);
        auto fend = bufOut.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),2.0F)) == fend);
      }
      
      AudioBuffer::transform<std::divides>(buf1,buf2,bufOut,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!bufOut.channelIsConstant[ch]);
        auto fbegin = bufOut.getChannelData(ch);
        auto fend = bufOut.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),2.0F)) == fend);
      }
    }
  }
 
  #pragma mark test inplace arithmetics between constant and non constant buffer
  {
    const size_t numSamples = 1000000;
    AudioBuffer buf1;
    AudioBuffer buf2;
    for (size_t i = 1; i < 10; ++i) {
      size_t numberOfChannels1 = i;
      size_t numberOfChannels2 = i > 5 ? 1 : i;
      size_t numberOfChannelsOut = std::max(numberOfChannels1, numberOfChannels2);
      buf1.resize(numberOfChannels1, numSamples);
      buf1.setIsConstant(true, 2.0F);
      buf2.resize(numberOfChannels2, numSamples);
      buf2.fill(1.0F, numSamples);
      buf2.setIsConstant(false);
      
      buf1.transform<std::plus>(buf2,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!buf1.channelIsConstant[ch]);
        auto fbegin = buf1.getChannelData(ch);
        auto fend = buf1.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),3.0F)) == fend);
      }
      
      buf1.setIsConstant(true, 2.0F);
      buf1.transform<std::minus>(buf2,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!buf1.channelIsConstant[ch]);
        auto fbegin = buf1.getChannelData(ch);
        auto fend = buf1.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),1.0F)) == fend);
      }
      
      buf1.setIsConstant(true, 2.0F);
      buf1.transform<std::multiplies>(buf2,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!buf1.channelIsConstant[ch]);
        auto fbegin = buf1.getChannelData(ch);
        auto fend = buf1.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),2.0F)) == fend);
      }
      
      buf1.setIsConstant(true, 2.0F);
      buf1.transform<std::divides>(buf2,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!buf1.channelIsConstant[ch]);
        auto fbegin = buf1.getChannelData(ch);
        auto fend = buf1.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),2.0F)) == fend);
      }
    }
  }
  
  #pragma mark test not inplace arithmetics between constant and constant buffer
  {
    const size_t numSamples = 1000000;
    AudioBuffer buf1;
    AudioBuffer buf2;
    AudioBuffer bufOut;
    for (size_t i = 1; i < 10; ++i) {
      size_t numberOfChannels1 = i > 3 ? ((i <= 5) ? 1 : i) : i;
      size_t numberOfChannels2 = i > 5 ? 1 : i;
      size_t numberOfChannelsOut = std::max(numberOfChannels1, numberOfChannels2);
      buf1.resize(numberOfChannels1, numSamples);
      buf1.setIsConstant(true, 2.0F);
      buf2.resize(numberOfChannels2, numSamples);
      buf2.fill(1.0F, numSamples);
      buf2.setIsConstant(false);
      bufOut.resize(numberOfChannelsOut, numSamples);
      
      AudioBuffer::transform<std::plus>(buf1,buf2,bufOut,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!bufOut.channelIsConstant[ch]);
        auto fbegin = bufOut.getChannelData(ch);
        auto fend = bufOut.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),3.0F)) == fend);
      }
      
      AudioBuffer::transform<std::minus>(buf1,buf2,bufOut,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!bufOut.channelIsConstant[ch]);
        auto fbegin = bufOut.getChannelData(ch);
        auto fend = bufOut.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),1.0F)) == fend);
      }
      
      AudioBuffer::transform<std::multiplies>(buf1,buf2,bufOut,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!bufOut.channelIsConstant[ch]);
        auto fbegin = bufOut.getChannelData(ch);
        auto fend = bufOut.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),2.0F)) == fend);
      }
      
      AudioBuffer::transform<std::divides>(buf1,buf2,bufOut,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!bufOut.channelIsConstant[ch]);
        auto fbegin = bufOut.getChannelData(ch);
        auto fend = bufOut.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),2.0F)) == fend);
      }
    }
  }
 
  #pragma mark test inplace arithmetics between non constant buffers
  {
    const size_t numSamples = 1000000;
    AudioBuffer buf1;
    AudioBuffer buf2;
    for (size_t i = 1; i < 10; ++i) {
      size_t numberOfChannels1 = i;
      size_t numberOfChannels2 = i > 5 ? 1 : i;
      size_t numberOfChannelsOut = std::max(numberOfChannels1, numberOfChannels2);
      buf1.resize(numberOfChannels1, numSamples);
      buf1.fill(2.0F, numSamples);
      buf1.setIsConstant(false);
      buf2.resize(numberOfChannels2, numSamples);
      buf2.fill(1.0F, numSamples);
      buf2.setIsConstant(false);
      
      buf1.transform<std::plus>(buf2,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!buf1.channelIsConstant[ch]);
        auto fbegin = buf1.getChannelData(ch);
        auto fend = buf1.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),3.0F)) == fend);
      }
      
      buf1.fill(2.0F, numSamples);
      buf1.setIsConstant(false);
      buf1.transform<std::minus>(buf2,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!buf1.channelIsConstant[ch]);
        auto fbegin = buf1.getChannelData(ch);
        auto fend = buf1.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),1.0F)) == fend);
      }
      
      buf1.fill(2.0F, numSamples);
      buf1.setIsConstant(false);
      buf1.transform<std::multiplies>(buf2,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!buf1.channelIsConstant[ch]);
        auto fbegin = buf1.getChannelData(ch);
        auto fend = buf1.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),2.0F)) == fend);
      }
      
      buf1.fill(2.0F, numSamples);
      buf1.setIsConstant(false);
      buf1.transform<std::divides>(buf2,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!buf1.channelIsConstant[ch]);
        auto fbegin = buf1.getChannelData(ch);
        auto fend = buf1.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),2.0F)) == fend);
      }
    }
  }
  
  #pragma mark test not inplace arithmetics between non constant buffers
  {
    const size_t numSamples = 1000000;
    AudioBuffer buf1;
    AudioBuffer buf2;
    AudioBuffer bufOut;
    for (size_t i = 1; i < 10; ++i) {
      size_t numberOfChannels1 = i > 3 ? ((i <= 5) ? 1 : i) : i;
      size_t numberOfChannels2 = i > 5 ? 1 : i;
      size_t numberOfChannelsOut = std::max(numberOfChannels1, numberOfChannels2);
      buf1.resize(numberOfChannels1, numSamples);
      buf1.fill(2.0F, numSamples);
      buf1.setIsConstant(false);
      buf2.resize(numberOfChannels2, numSamples);
      buf2.fill(1.0F, numSamples);
      buf2.setIsConstant(false);
      bufOut.resize(numberOfChannelsOut, numSamples);
      
      AudioBuffer::transform<std::plus>(buf1,buf2,bufOut,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!bufOut.channelIsConstant[ch]);
        auto fbegin = bufOut.getChannelData(ch);
        auto fend = bufOut.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),3.0F)) == fend);
      }
      
      AudioBuffer::transform<std::minus>(buf1,buf2,bufOut,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!bufOut.channelIsConstant[ch]);
        auto fbegin = bufOut.getChannelData(ch);
        auto fend = bufOut.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),1.0F)) == fend);
      }
      
      AudioBuffer::transform<std::multiplies>(buf1,buf2,bufOut,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!bufOut.channelIsConstant[ch]);
        auto fbegin = bufOut.getChannelData(ch);
        auto fend = bufOut.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),2.0F)) == fend);
      }
      
      AudioBuffer::transform<std::divides>(buf1,buf2,bufOut,numSamples);
      assert(buf1.channels == numberOfChannels1);
      assert(buf1.usedChannels == numberOfChannels1);
      assert(buf1.size == numSamples);
      assert(buf2.channels == numberOfChannels2);
      assert(buf2.usedChannels == numberOfChannels2);
      assert(buf2.size == numSamples);
      for (int ch = 0; ch < i; ++ch) {
        assert(!bufOut.channelIsConstant[ch]);
        auto fbegin = bufOut.getChannelData(ch);
        auto fend = bufOut.getChannelData(ch) + numSamples;
        assert(std::find_if(fbegin, fend, std::bind2nd(std::not_equal_to<float>(),2.0F)) == fend);
      }
    }
  }
  std::cout << "Test is finished successfully" << std::endl;
  return 0;
}


