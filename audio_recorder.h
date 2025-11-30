#pragma once

#include "portaudio.h"
#include "utils.h"

class AudioRecorder {
public:
    explicit AudioRecorder(AudioData* data) : audioData(data) {}

    PaError start();
    PaError stop();

private:
    AudioData* audioData;  // not owning
    PaDeviceIndex findLoopbackDevice();
};