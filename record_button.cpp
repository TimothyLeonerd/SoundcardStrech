#include "record_button.h"
#include "wave_panel.h"
#include <iostream>
#include <cstring>  // for memset
#include "record.h"
#include "playback.h"
#include "portaudio.h"
#include "state.h"
#include "main_window.h"
#include "my_events.h"

wxBEGIN_EVENT_TABLE(Record_Button, wxPanel)
EVT_TIMER(wxID_ANY, Record_Button::OnTimer)
wxEND_EVENT_TABLE()

Record_Button::Record_Button(
    wxWindow* parent,
    std::shared_ptr<State> pState,
    std::shared_ptr<AudioData> pData)
    : wxPanel(parent, wxID_ANY),
    pStateCpy(pState),
    pDataCpy(pData),
    stream(nullptr),
    m_timer(this) // Timer constructed with 'this' as the owner
{
    const int ID_RECORD_BUTTON = wxNewId();
    button = new wxButton(this, ID_RECORD_BUTTON, wxT(""));

    recordBundle = wxBitmapBundle::FromSVGFile("icons/record.svg", wxSize(24, 24));
    if (recordBundle.IsOk()) { button->SetBitmap(recordBundle); button->SetToolTip("Record"); }

    auto* s = new wxBoxSizer(wxHORIZONTAL);
    s->Add(button, 0, 0, 0);
    SetSizerAndFit(s);

    stopBundle = wxBitmapBundle::FromSVGFile("icons/stop.svg", wxSize(24, 24));

    // OnRecord(...) runs when user clicks button
    button->Bind(wxEVT_BUTTON, &Record_Button::OnRecord, this);

    // OnTimer(...) runs when m_timer fires
    // (using the timer's ID, which is m_timer.GetId()).
    this->Bind(wxEVT_TIMER, &Record_Button::OnTimer, this, m_timer.GetId());

    button->SetFocus();
    Centre();
}

// The same existing OnRecord(...) but with added timer logic
void Record_Button::OnRecord(wxCommandEvent& WXUNUSED(event))
{
    PaError err = paNoError;
    int totalFrames;
    int numSamples;
    int numBytes;
    int n_devices, idx_output_device;
    std::string output_device_name;
    std::vector<const PaDeviceInfo*> all_devices;
    std::vector<const PaDeviceInfo*> devices_matching_str;
    const std::string loopback_str = "[Loopback]";
    PaStreamParameters inputParameters = {};

    // If currently Idle, user pressed "Record" -> Start recording
    if (pStateCpy->state == Idle)
    {
        button->SetBitmap(stopBundle);
        button->SetToolTip("Stop Recording");
        pStateCpy->transition(Recording);

        wxWindow* top = wxGetTopLevelParent(this);
        if (top)
        {
            MainWindow* mw = dynamic_cast<MainWindow*>(top);
            if (mw)
            {
                mw->playButton->button->Enable(false);
                wxCommandEvent event(myEVT_RECORD_STARTED);
                wxPostEvent(mw->playButton, event);
                wxPostEvent(mw->wavePanel, event);
            }
            else
            {
                // not actually MainWindow
                wxMessageBox("top is not a MainWindow");
            }
        }
        

        /* Set parameters */
        pDataCpy->maxFrameIndex = totalFrames = NUM_SECONDS * SAMPLE_RATE;
        pDataCpy->currentSampleIndex = 0;
        numSamples = totalFrames * NUM_CHANNELS;
        numBytes = numSamples * sizeof(SAMPLE);

        /* Init recorded samples buffer */
        pDataCpy->recorded = (SAMPLE*)std::malloc(numBytes);
        if (pDataCpy->recorded == NULL)
        {
            std::cerr << "Could not allocate record array.\n";
            goto error;
        }
        std::memset(pDataCpy->recorded, 0, numBytes);

        err = Pa_Initialize();
        if (err != paNoError) goto error;

        n_devices = Pa_GetDeviceCount();
        idx_output_device = Pa_GetDefaultOutputDevice();
        output_device_name = Pa_GetDeviceInfo(idx_output_device)->name;

        // For default output device, find corresponding [Loopback] channel
        for (int i = 0; i < n_devices; ++i)
        {
            all_devices.push_back(Pa_GetDeviceInfo(i));
            if (std::string(all_devices[i]->name).find(output_device_name) != std::string::npos) {
                if (std::string(all_devices[i]->name).find(loopback_str) != std::string::npos) {
                    inputParameters.device = i;
                }
            }
        }
        if (inputParameters.device == paNoDevice) {
            std::cerr << "Error: No default input device.\n";
            goto error;
        }
        inputParameters.channelCount = NUM_CHANNELS;
        inputParameters.sampleFormat = PA_SAMPLE_TYPE;
        inputParameters.suggestedLatency =
            Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
        inputParameters.hostApiSpecificStreamInfo = NULL;

        err = Pa_OpenStream(
            &stream,
            &inputParameters,
            NULL,  /* no output device */
            Pa_GetDeviceInfo(inputParameters.device)->defaultSampleRate,
            FRAMES_PER_BUFFER,
            paClipOff,
            recordCallback,
            pDataCpy.get()
        );
        if (err != paNoError) goto error;

        err = Pa_StartStream(stream);
        if (err != paNoError) goto error;

        m_timer.Start(1);

        goto done;

    error:
        Pa_Terminate();
        if (err != paNoError)
        {
            std::cerr << "An error occurred while using the portaudio stream\n"
                << "Error number: " << err << "\n"
                << "Error message: " << Pa_GetErrorText(err) << std::endl;
            err = 1;
        }
    done:
        ;
    }
    // If currently Recording, user pressed "Record" again -> Stop recording early
    else if (pStateCpy->state == Recording)
    {
        button->SetBitmap(recordBundle);
        button->SetToolTip("Record");
        pStateCpy->transition(Idle);

        // Stop checking (no need to poll once user stops)
        m_timer.Stop();

        // Close the stream
        if (stream) {
            PaError err = Pa_CloseStream(stream);
            if (err != paNoError) {
                std::cerr << "Pa_CloseStream error: " << Pa_GetErrorText(err) << std::endl;
            }
            else {
                // We only recorded up to pDataCpy->currentSampleIndex
                // Adjust maxFrameIndex so playback doesn’t exceed that
                pDataCpy->maxFrameIndex = pDataCpy->currentSampleIndex;
            }
            stream = nullptr;
        }
        Pa_Terminate();

        wxWindow* top = wxGetTopLevelParent(this);
        if (top)
        {
            MainWindow* mw = dynamic_cast<MainWindow*>(top);
            if (mw)
            {
                mw->playButton->button->Enable(true);
                wxCommandEvent event(myEVT_RECORD_STOPPED);
                wxPostEvent(mw, event);
            }
            else
            {
                // not actually MainWindow
                wxMessageBox("top is not a MainWindow");
            }
        }

    }
}

// NEW: Timer event handler that checks if the stream is still active
void Record_Button::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    if (!stream) return; // Safety check in case stream is null

    PaError err;
    int active = Pa_IsStreamActive(stream);
    if (active == 0)
    {
        // The callback signaled paComplete => buffer is full or done
        // Stop the timer
        m_timer.Stop();

        // Close the stream
        err = Pa_CloseStream(stream);
        if (err != paNoError) {
            std::cerr << "Pa_CloseStream error: "
                << Pa_GetErrorText(err) << std::endl;
        }
        stream = nullptr;

        Pa_Terminate();

        // Transition to Idle, reset button label
        button->SetBitmap(recordBundle);
        button->SetToolTip("Record");
        pStateCpy->transition(Idle);
    }
    else if (active < 0)
    {
        // Some error occurred in the callback
        std::cerr << "Stream error: " << Pa_GetErrorText(active) << std::endl;
        // Also stop timer, close stream, revert to idle
        m_timer.Stop();
        err = Pa_CloseStream(stream);
        stream = nullptr;
        Pa_Terminate();
        button->SetBitmap(recordBundle);
        button->SetToolTip("Record");
        pStateCpy->transition(Idle);
    }
    else
    {
        wxWindow* top = wxGetTopLevelParent(this);
        if (top)
        {
            MainWindow* mw = dynamic_cast<MainWindow*>(top);
            if (mw)
            {
                wxCommandEvent event(myEVT_DRAW_SCREEN);
                wxPostEvent(mw, event);
            }
            else
            {
                // not actually MainWindow
                wxMessageBox("top is not a MainWindow");
            }
        }
    }
}
