#include "audio_recorder.h"
#include "portaudio.h"
#include <string>

/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
int recordCallback(
    const void* inputBuffer,
    void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
    AudioData* data = (AudioData*)userData;
    const SAMPLE* rptr = (const SAMPLE*)inputBuffer;
    SAMPLE* wptr = &data->recorded[data->currentSampleIndex * NUM_CHANNELS];
    long framesToCalc;
    long i;
    int finished;
    unsigned long framesLeft = data->maxSamplesBuffer - data->currentSampleIndex;

    (void)outputBuffer; /* Prevent unused variable warnings. */
    (void)timeInfo;
    (void)statusFlags;
    (void)userData;

    if (framesLeft < framesPerBuffer)
    {
        framesToCalc = framesLeft;
        finished = paComplete;
    }
    else
    {
        framesToCalc = framesPerBuffer;
        finished = paContinue;
    }

    if (inputBuffer == NULL)
    {
        for (i = 0; i < framesToCalc; i++)
        {
            *wptr++ = SAMPLE_SILENCE;  /* left */
            if (NUM_CHANNELS == 2) *wptr++ = SAMPLE_SILENCE;  /* right */
        }
    }
    else
    {
        for (i = 0; i < framesToCalc; i++)
        {
            *wptr++ = *rptr++;  /* left */
            if (NUM_CHANNELS == 2) *wptr++ = *rptr++;  /* right */
        }
    }
    data->currentSampleIndex += framesToCalc;
    return finished;
}

PaDeviceIndex AudioRecorder::findLoopbackDevice()
{
    PaDeviceIndex loopbackDevice_idx = paNoDevice;

    int n_devices = Pa_GetDeviceCount();
    PaDeviceIndex idx_output_device = Pa_GetDefaultOutputDevice();
    std::string output_device_name = Pa_GetDeviceInfo(idx_output_device)->name;

    for (int i = 0; i < n_devices; ++i)
    {
        std::string current_device_name = Pa_GetDeviceInfo(i)->name;
        if (current_device_name.find(output_device_name) != std::string::npos) {
            if (current_device_name.find("[Loopback]") != std::string::npos) {
                loopbackDevice_idx = i;
                break;
            }
        }
    }

    return loopbackDevice_idx;
}

PaError AudioRecorder::start() {
    PaError err = paNoError;

    err = Pa_Initialize();
    if (err != paNoError) return err;

    PaStreamParameters inputParameters = {};
    inputParameters.device = findLoopbackDevice();
    if (inputParameters.device == paNoDevice) {
        return paDeviceUnavailable;
    }

    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency =
        Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
        &audioData->stream,
        &inputParameters,
        NULL,
        Pa_GetDeviceInfo(inputParameters.device)->defaultSampleRate,
        FRAMES_PER_BUFFER,
        paClipOff,
        recordCallback,
        audioData
    );
    if (err != paNoError) return err;

    err = Pa_StartStream(audioData->stream);
    if (err != paNoError) return err;

    return paNoError;
}

PaError AudioRecorder::stop()
{
    PaError err = paNoError;

    if (audioData->stream) {
        err = Pa_CloseStream(audioData->stream);
        audioData->stream = nullptr;
    }

    err = Pa_Terminate();
    return err;
}