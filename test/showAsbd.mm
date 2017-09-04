#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioToolbox/ExtendedAudioFile.h>

void AUM_printAvailableStreamFormatsForId(AudioFileTypeID fileTypeID, UInt32 mFormatID)
{
    AudioFileTypeAndFormatID fileTypeAndFormat;
    fileTypeAndFormat.mFileType = fileTypeID;
    fileTypeAndFormat.mFormatID = mFormatID;
    UInt32 fileTypeIDChar = CFSwapInt32HostToBig(fileTypeID);
    UInt32 mFormatChar = CFSwapInt32HostToBig(mFormatID);

    OSStatus audioErr = noErr;
    UInt32 infoSize = 0;
    audioErr = AudioFileGetGlobalInfoSize(kAudioFileGlobalInfo_AvailableStreamDescriptionsForFormat,
                                          sizeof (fileTypeAndFormat),
                                          &fileTypeAndFormat,
                                          &infoSize);
    if (audioErr != noErr) {
        UInt32 format4cc = CFSwapInt32HostToBig(audioErr);
        NSLog(@"-: fileTypeID: %4.4s, mFormatId: %4.4s, not supported (%4.4s)",
              //i,
              (char*)&fileTypeIDChar,
              (char*)&mFormatChar,
              (char*)&format4cc
              );

        return;
    }

    AudioStreamBasicDescription *asbds = (AudioStreamBasicDescription*)malloc (infoSize);
    audioErr = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_AvailableStreamDescriptionsForFormat,
                                      sizeof (fileTypeAndFormat),
                                      &fileTypeAndFormat,
                              	        &infoSize,
                                      asbds);
    if (audioErr != noErr) {
        UInt32 format4cc = CFSwapInt32HostToBig(audioErr);
        NSLog(@"-: fileTypeID: %4.4s, mFormatId: %4.4s, not supported (%4.4s)",
              //i,
              (char*)&fileTypeIDChar,
              (char*)&mFormatChar,
              (char*)&format4cc
              );

        return;
    }

    int asbdCount = infoSize / sizeof (AudioStreamBasicDescription);
    for (int i=0; i<asbdCount; i++) {
        UInt32 format4cc = CFSwapInt32HostToBig(asbds[i].mFormatID);

        NSLog(@"%d: fileTypeID: %4.4s, mFormatId: %4.4s, mFormatFlags: %d, mBitsPerChannel: %d",
              i,
              (char*)&fileTypeIDChar,
              (char*)&format4cc,
              (unsigned int)asbds[i].mFormatFlags,
              (unsigned int)asbds[i].mBitsPerChannel);
    }

    free (asbds);
}

int main (void)
{
    NSLog(@"********* CAF ***********");
    AUM_printAvailableStreamFormatsForId(kAudioFileCAFType, kAudioFormatAppleIMA4);
    AUM_printAvailableStreamFormatsForId(kAudioFileCAFType, kAudioFormatAC3);
    AUM_printAvailableStreamFormatsForId(kAudioFileCAFType, kAudioFormatMPEG4AAC);
    AUM_printAvailableStreamFormatsForId(kAudioFileCAFType, kAudioFormatAppleLossless);

    NSLog(@"********* AIFF ***********");
    AUM_printAvailableStreamFormatsForId(kAudioFileAIFFType, kAudioFormatLinearPCM);
    AUM_printAvailableStreamFormatsForId(kAudioFileAIFFType, kAudioFormatAppleIMA4);
    AUM_printAvailableStreamFormatsForId(kAudioFileAIFFType, kAudioFormatAC3);
    AUM_printAvailableStreamFormatsForId(kAudioFileAIFFType, kAudioFormatMPEG4AAC);
    AUM_printAvailableStreamFormatsForId(kAudioFileAIFFType, kAudioFormatAppleLossless);

    NSLog(@"********* M4A ***********");
    AUM_printAvailableStreamFormatsForId(kAudioFileM4AType, kAudioFormatAppleIMA4);
    AUM_printAvailableStreamFormatsForId(kAudioFileM4AType, kAudioFormatAC3);
    AUM_printAvailableStreamFormatsForId(kAudioFileM4AType, kAudioFormatMPEG4AAC);
    AUM_printAvailableStreamFormatsForId(kAudioFileM4AType, kAudioFormatAppleLossless);

    NSLog(@"********* AAC_ADTS ***********");
    AUM_printAvailableStreamFormatsForId(kAudioFileAAC_ADTSType, kAudioFormatLinearPCM);
    AUM_printAvailableStreamFormatsForId(kAudioFileAAC_ADTSType, kAudioFormatAppleIMA4);
    AUM_printAvailableStreamFormatsForId(kAudioFileAAC_ADTSType, kAudioFormatAC3);
    AUM_printAvailableStreamFormatsForId(kAudioFileAAC_ADTSType, kAudioFormatMPEG4AAC);
    AUM_printAvailableStreamFormatsForId(kAudioFileAAC_ADTSType, kAudioFormatAppleLossless);

    NSLog(@"********* AIFC ***********");
    AUM_printAvailableStreamFormatsForId(kAudioFileAIFCType, kAudioFormatLinearPCM);
    AUM_printAvailableStreamFormatsForId(kAudioFileAIFCType, kAudioFormatAppleIMA4);
    AUM_printAvailableStreamFormatsForId(kAudioFileAIFCType, kAudioFormatAC3);
    AUM_printAvailableStreamFormatsForId(kAudioFileAIFCType, kAudioFormatMPEG4AAC);
    AUM_printAvailableStreamFormatsForId(kAudioFileAIFCType, kAudioFormatAppleLossless);
}