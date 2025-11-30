#include "play_button.h"
#include <iostream>
#include <cstring>    // For memset
#include "playback.h" // For playCallback
#include "portaudio.h"
#include "state.h"
#include "RubberBandStretcher.h"
#include "main_window.h"
#include "wave_panel.h"
#include "my_events.h"

wxBEGIN_EVENT_TABLE(Play_Button, wxPanel)
wxEND_EVENT_TABLE()

Play_Button::Play_Button(
    wxWindow* parent,
    std::shared_ptr<State> pState,
    std::shared_ptr<AudioData> pData)
    : wxPanel(parent, wxID_ANY),
    pStateCpy(pState),
    pAudioData(pData),
    stream(nullptr),
    m_timer(this) // Timer owned by this frame
{
    // Create the actual wxButton
    int ID_PLAY_BUTTON = wxNewId();
    button = new wxButton(this, ID_PLAY_BUTTON, wxT(""));

    playBundle = wxBitmapBundle::FromSVGFile("icons/play_arrow.svg", wxSize(24, 24));
    if (playBundle.IsOk()) { button->SetBitmap(playBundle); button->SetToolTip("Play"); }

    pauseBundle = wxBitmapBundle::FromSVGFile("icons/pause.svg", wxSize(24, 24));

    auto* s = new wxBoxSizer(wxHORIZONTAL);
    s->Add(button, 0, 0, 0);
    SetSizerAndFit(s);

    // When button is clicked, call OnPlay(...)
    button->Bind(wxEVT_BUTTON, &Play_Button::OnPlay, this);
    this->Bind(myEVT_RECORD_STARTED, &Play_Button::OnRecord, this);

    // The timer calls OnTimer(...) when it goes off
    this->Bind(wxEVT_TIMER, &Play_Button::OnTimer, this, m_timer.GetId());

    button->SetFocus();
    Centre();
}

void::Play_Button::OnRecord(wxCommandEvent& event) {
    button->Enable(false);
    //button->SetBitmap(wxBitmapBundle::FromSVGFile("icons/play_arrow_grey.svg", wxSize(24, 24)));
}

void Play_Button::OnPlay(wxCommandEvent& WXUNUSED(event))
{
    PaError err;

    // If buffer allocated or no samples recorded
    if (pAudioData->totalSamplesRecorded == 0 || pAudioData->maxSamplesBuffer == 0) {
        wxMessageBox("No recorded data to play!", "Info");
        return;
    }

    // OFFLINE APPROACH:
    {
        // 1) Create a RubberBandStretcher in offline mode
        RubberBand::RubberBandStretcher stretcher(
            SAMPLE_RATE,
            NUM_CHANNELS,
            RubberBand::RubberBandStretcher::OptionProcessOffline  // Offline
        );

        double ratio = 1.0;
        if (pStateCpy) {
            ratio = pStateCpy->timeRatio;
        }

        // Clamp to a sensible range just in case
        if (ratio < 0.5) ratio = 0.5;
        if (ratio > 2.0) ratio = 2.0;

        stretcher.setTimeRatio(ratio);

        // 3) Prepare to pass entire recorded buffer as a single chunk
        //    i.e., one "study" pass, then one "process" pass, then retrieve.
        int frames = pAudioData->totalSamplesRecorded; // how many frames we actually recorded
        int channels = NUM_CHANNELS;

        // Interleaved input => must "de-interleave" for Rubber Band's offline calls
        // i.e. float *const *input
        // We'll make a vector of vectors: inputPlanar[c][frame]
        std::vector<std::vector<float>> inputPlanar(channels, std::vector<float>(frames));
        for (int f = 0; f < frames; f++) {
            for (int c = 0; c < channels; c++) {
                // interleaved index = f*channels + c
                inputPlanar[c][f] = pAudioData->recorded[f * channels + c];
            }
        }

        // RubberBand wants an array of pointers to each channel
        std::vector<const float*> inputPtrs(channels);
        for (int c = 0; c < channels; c++) {
            inputPtrs[c] = inputPlanar[c].data();
        }

        // 4) "study" pass: tell it about the entire audio
        //    final=true => we have no more data
        stretcher.study(inputPtrs.data(), frames, true);

        // 5) "process" pass: pass the same data again
        stretcher.process(inputPtrs.data(), frames, true);

        // 6) Retrieve all the stretched frames
        int available = stretcher.available(); // how many frames are stretched out
        std::vector<std::vector<float>> outputPlanar(channels, std::vector<float>(available));
        // gather pointers to each channel
        std::vector<float*> outPtrs(channels);
        for (int c = 0; c < channels; c++) {
            outPtrs[c] = outputPlanar[c].data();
        }

        // read them
        pAudioData->totalSamplesRecorded = stretcher.retrieve((float* const*)outPtrs.data(), available);

        // 7) Re-interleave them back into pAudioData->recorded
        //    so the existing callback can just play them
        //    We'll free the old pointer and allocate a new sized buffer
        free(pAudioData->recorded);
        pAudioData->recorded = (SAMPLE*)malloc(pAudioData->totalSamplesRecorded * channels * sizeof(SAMPLE));
        if (!pAudioData->recorded) {
            wxMessageBox("Allocation failed for stretched buffer!", "Error");
            return;
        }

        // re-interleave
        for (int f = 0; f < pAudioData->totalSamplesRecorded; f++) {
            for (int c = 0; c < channels; c++) {
                pAudioData->recorded[f * channels + c] = outputPlanar[c][f];
            }
        }

        // update currentSampleIndex / maxFrameIndex
        pAudioData->currentSampleIndex = 0;         // will start from beginning
    }

    // At this point, pAudioData->recorded holds the new time-stretched data
    // pAudioData->maxFrameIndex is updated to the new length

    // Proceed with your existing playback code (Case 1)...

    if (pStateCpy->state == Idle)
    {
        // Switch to "Playing" state and label
        pStateCpy->transition(Playing);

        // Notify WavePanel that playback started
        wxWindow* top = wxGetTopLevelParent(this);
        if (top)
        {
            if (auto mw = dynamic_cast<MainWindow*>(top))
            {
                if (mw->wavePanel)
                {
                    wxCommandEvent ev(myEVT_PLAY_STARTED);
                    wxPostEvent(mw->wavePanel, ev);
                }
            }
        }

        button->SetBitmap(pauseBundle);
        button->SetToolTip("Pause");

        // Reset to start of newly stretched data
        pAudioData->currentSampleIndex = 0;

        err = Pa_Initialize();
        if (err != paNoError) {
            std::cerr << "Pa_Initialize error: " << Pa_GetErrorText(err) << std::endl;
            return;
        }

        PaStreamParameters outputParams;
        std::memset(&outputParams, 0, sizeof(outputParams));

        outputParams.device = Pa_GetDefaultOutputDevice();
        if (outputParams.device == paNoDevice) {
            std::cerr << "Error: No default output device.\n";
            goto error;
        }
        outputParams.channelCount = NUM_CHANNELS;
        outputParams.sampleFormat = PA_SAMPLE_TYPE;
        outputParams.suggestedLatency =
            Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
        outputParams.hostApiSpecificStreamInfo = NULL;

        err = Pa_OpenStream(
            &stream,
            NULL,
            &outputParams,
            SAMPLE_RATE,
            FRAMES_PER_BUFFER,
            paClipOff,
            playCallback,
            pAudioData.get()
        );
        if (err != paNoError) goto error;

        err = Pa_StartStream(stream);
        if (err != paNoError) goto error;

        m_timer.Start(200);
        return;

    error:
        std::cerr << "Pa_OpenStream/Pa_StartStream error: "
            << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        // Notify WavePanel that playback stopped
        {
            wxWindow* top = wxGetTopLevelParent(this);
            if (top)
            {
                if (auto mw = dynamic_cast<MainWindow*>(top))
                {
                    if (mw->wavePanel)
                    {
                        wxCommandEvent evStop(myEVT_PLAY_STOPPED);
                        wxPostEvent(mw->wavePanel, evStop);
                    }
                }
            }
        }
        pStateCpy->transition(Idle);
        button->SetBitmap(playBundle);
        button->SetToolTip("Play");
    }
    else if (pStateCpy->state == Playing)
    {
        // existing stop logic...
        pStateCpy->transition(Idle);
        button->SetBitmap(playBundle);
        button->SetToolTip("Play");
        // Notify WavePanel that playback stopped
        {
            wxWindow* top = wxGetTopLevelParent(this);
            if (top)
            {
                if (auto mw = dynamic_cast<MainWindow*>(top))
                {
                    if (mw->wavePanel)
                    {
                        wxCommandEvent evStop(myEVT_PLAY_STOPPED);
                        wxPostEvent(mw->wavePanel, evStop);
                    }
                }
            }
        }
        m_timer.Stop();
        if (stream) {
            err = Pa_CloseStream(stream);
            if (err != paNoError) {
                std::cerr << "Pa_CloseStream error: "
                    << Pa_GetErrorText(err) << std::endl;
            }
            Pa_Terminate();
            stream = nullptr;
        }
    }
}

// Called periodically to see if playback finished
void Play_Button::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    if (!stream) return; // safety check

    int active = Pa_IsStreamActive(stream);
    if (active == 0)
    {
        // Playback is done
        m_timer.Stop();

        PaError err = Pa_CloseStream(stream);
        if (err != paNoError) {
            std::cerr << "Pa_CloseStream error: "
                << Pa_GetErrorText(err) << std::endl;
        }
        Pa_Terminate();
        stream = nullptr;

        // Notify WavePanel that playback stopped
        {
            wxWindow* top = wxGetTopLevelParent(this);
            if (top)
            {
                if (auto mw = dynamic_cast<MainWindow*>(top))
                {
                    if (mw->wavePanel)
                    {
                        wxCommandEvent evStop(myEVT_PLAY_STOPPED);
                        wxPostEvent(mw->wavePanel, evStop);
                    }
                }
            }
        }

        pStateCpy->transition(Idle);
        button->SetBitmap(playBundle);
        button->SetToolTip("Play");
    }
    else if (active < 0)
    {
        // An error occurred in the callback
        std::cerr << "Stream error: "
            << Pa_GetErrorText(active) << std::endl;

        m_timer.Stop();
        PaError err = Pa_CloseStream(stream);
        stream = nullptr;
        Pa_Terminate();

        // Notify WavePanel that playback stopped
        {
            wxWindow* top = wxGetTopLevelParent(this);
            if (top)
            {
                if (auto mw = dynamic_cast<MainWindow*>(top))
                {
                    if (mw->wavePanel)
                    {
                        wxCommandEvent evStop(myEVT_PLAY_STOPPED);
                        wxPostEvent(mw->wavePanel, evStop);
                    }
                }
            }
        }

        pStateCpy->transition(Idle);
        button->SetBitmap(playBundle);
        button->SetToolTip("Play");
    }
}
