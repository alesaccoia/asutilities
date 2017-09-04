//
//  AudioFormat_sndfile.hpp
//
//  Created by Alessandro Saccoia on 1/19/15.
//
//

#ifndef __AudioFormat_sndfile_
#define __AudioFormat_sndfile_

#include "AudioFormat.hpp"

namespace dinahmoe {
namespace assets {

class AudioFormat_sndfile : public AudioFormat {
public:
  AudioFormat_sndfile();

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

};

}
}

#endif /* defined(__AudioFormat_sndfile_) */
