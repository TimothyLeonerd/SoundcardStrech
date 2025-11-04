#include "playback.h"

int playCallback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
    AudioData* data = (AudioData*)userData;
    SAMPLE* rptr = &data->recorded[data->currentSampleIndex * NUM_CHANNELS];
    SAMPLE* wptr = (SAMPLE*)outputBuffer;
    unsigned int i;
    int finished;
    unsigned int framesLeft = data->maxFrameIndex - data->currentSampleIndex;

    (void)inputBuffer; /* Prevent unused variable warnings. */
    (void)timeInfo;
    (void)statusFlags;
    (void)userData;

    if (framesLeft < framesPerBuffer)
    {
        /* final buffer... */
        for (i = 0; i < framesLeft; i++)
        {
            *wptr++ = *rptr++;  /* left */
            if (NUM_CHANNELS == 2) *wptr++ = *rptr++;  /* right */
        }
        for (; i < framesPerBuffer; i++)
        {
            *wptr++ = 0;  /* left */
            if (NUM_CHANNELS == 2) *wptr++ = 0;  /* right */
        }
        data->currentSampleIndex += framesLeft;
        finished = paComplete;
    }
    else
    {
        for (i = 0; i < framesPerBuffer; i++)
        {
            *wptr++ = *rptr++;  /* left */
            if (NUM_CHANNELS == 2) *wptr++ = *rptr++;  /* right */
        }
        data->currentSampleIndex += framesPerBuffer;
        finished = paContinue;
    }
    return finished;
}