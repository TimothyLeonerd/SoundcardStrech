#include "utils.h"
#include <iostream>
#include <cstring>

using namespace std;

AudioData::AudioData()
    : lastSampleIndex(0),
    currentSampleIndex(0),
    totalSamplesRecorded(0),
    maxSamplesBuffer(0),
    recorded(nullptr) {

    // Alloc mem
    maxSamplesBuffer = NUM_SECONDS * SAMPLE_RATE;
    int numSamples = maxSamplesBuffer * NUM_CHANNELS;
    int numBytes = numSamples * sizeof(SAMPLE);

    /* Init recorded samples buffer */
    recorded = (SAMPLE*)std::malloc(numBytes);
    if (recorded == NULL)
    {
        std::cerr << "Could not allocate record array.\n";
    }
    std::memset(recorded, 0, numBytes);
}