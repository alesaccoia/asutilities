
#include <iostream>
#include <chrono>
#include "AudioFileIO.hpp"

#define FILENAME "test.aif"

using namespace dinahmoe;
using namespace audioIo;

int main (int argc, char** argv) {
  AudioBuffer outBuf(1,1000000);
  outBuf.createNoise(-0.9, 0.9);
  float samplingRate = 44100.F, samplingRate2 = 0.F;
  
  if (!writeFile (FILENAME, outBuf, samplingRate, DM_FORMAT_AIFF)) {
    std::cerr << "Problem with output file" << std::endl;
    return 1;
  }
  
  AudioBuffer inBuf;
  if (!loadFile(FILENAME, inBuf, samplingRate2)) {
    std::cerr << "Problem with input file" << std::endl;
    return 1;
  }
  
  system("rm -rf " FILENAME);
  
  if (inBuf.size != outBuf.size) {
    return 1;
  }
  if (samplingRate != samplingRate2) {
    return 1;
  }
  
  float maxAbsError = 1.0F / (float)(1 << 14);
  
  float avgError = .0F;
  float maxError = .0F;
  float err = .0F;
  std::cout << "Max of generated file = " << *std::max_element(outBuf.data[0], outBuf.data[0] + outBuf.size) << std::endl;
  std::cout << "Max of read file = " << *std::max_element(inBuf.data[0], inBuf.data[0] + inBuf.size) << std::endl;
  for (int i = 0; i < inBuf.size; ++i) {
    err = fabs(inBuf.data[0][i] - outBuf.data[0][i]);
    if (err > maxError)
      maxError = err;
    avgError += err;
  }
  avgError /= (float)inBuf.size;
  std::cerr << "Max error = " << maxError << std::endl;
  std::cerr << "Avg error = " << avgError << std::endl;
  std::cerr << "Tolerable error = " << maxAbsError << std::endl;
 
  if (maxError > maxAbsError) {;
    return 1;
  }
 
  return 0;
}


