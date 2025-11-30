#pragma once
#include "portaudio.h"

#define RECORD_STR "Record"
#define RECORDING_STR "Recording"

#define PLAY_STR "Play"
#define PLAYING_STR "Playing"

/* #define SAMPLE_RATE  (17932) // Test failure to open with this value. */
#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (512)
#define NUM_SECONDS     (100)
#define NUM_CHANNELS    (2)
/* #define DITHER_FLAG     (paDitherOff) */
#define DITHER_FLAG     (0) /**/
/** Set to 1 if you want to capture the recording to a file. */
#define WRITE_TO_FILE   (0)

/* Select sample format. */
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif

// utils.h
class AudioData {
public:
    int lastSampleIndex = 0;
    int currentSampleIndex = 0;        // current read/write sample
    int totalSamplesRecorded = 0;      // total samples recorded
    int maxSamplesBuffer = 0;
    float* recorded = nullptr; // interleaved [frames * channels]
};
