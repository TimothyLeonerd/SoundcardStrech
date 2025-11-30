#include "record_button.h"
#include "wave_panel.h"
#include <iostream>
#include <cstring>  // for memset
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
    pAudioData(pData),
    m_timer(this) // Timer constructed with 'this' as the owner
{
    const int ID_RECORD_BUTTON = wxNewId();
    button = new wxButton(this, ID_RECORD_BUTTON, wxT(""));

    recordBundle = wxBitmapBundle::FromSVGFile("icons/record.svg", wxSize(24, 24));
    if (recordBundle.IsOk()) { button->SetBitmap(recordBundle); button->SetToolTip("Record"); }

    auto* s = new wxBoxSizer(wxHORIZONTAL);
    s->Add(button, 0, 0, 0);
    SetSizerAndFit(s);

    recorder = std::make_unique<AudioRecorder>(pAudioData.get());

    stopBundle = wxBitmapBundle::FromSVGFile("icons/stop.svg", wxSize(24, 24));

    // OnRecord(...) runs when user clicks button
    button->Bind(wxEVT_BUTTON, &Record_Button::OnRecord, this);

    // OnTimer(...) runs when m_timer fires
    // (using the timer's ID, which is m_timer.GetId()).
    this->Bind(wxEVT_TIMER, &Record_Button::OnTimer, this, m_timer.GetId());

    button->SetFocus();
    Centre();
}

void Record_Button::updateGuiRecordStarted() {
    button->SetBitmap(stopBundle);
    button->SetToolTip("Stop Recording");

    wxWindow* top = wxGetTopLevelParent(this);
    if (top)
    {
        MainWindow* mw = dynamic_cast<MainWindow*>(top);
        if (mw)
        {
            mw->playButton->button->Enable(false);
            wxCommandEvent e(myEVT_RECORD_STARTED);
            wxPostEvent(mw->playButton, e);
            wxPostEvent(mw->wavePanel, e);
        }
        else
        {
            // not actually MainWindow
            wxMessageBox("top is not a MainWindow");
        }
    }
}

PaDeviceIndex Record_Button::findLoopbackDecive() {
    PaDeviceIndex loopbackDevice_idx = paNoDevice;

    int n_devices = Pa_GetDeviceCount();
    PaDeviceIndex idx_output_device = Pa_GetDefaultOutputDevice();
    std::string output_device_name = Pa_GetDeviceInfo(idx_output_device)->name;

    // For default output device, find corresponding [Loopback] channel
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

// The same existing OnRecord(...) but with added timer logic
void Record_Button::OnRecord(wxCommandEvent& e)
{
    // If currently Idle, user pressed "Record" -> Start recording
    if (pStateCpy->state == Idle)
    {
        PaError err = recorder->start();
        if (err == paNoError) {
            m_timer.Start(1);
            pStateCpy->transition(Recording);
            updateGuiRecordStarted();
        }
        else {
            std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        }
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
        if (pAudioData->stream) {
            PaError err = Pa_CloseStream(pAudioData->stream);
            if (err != paNoError) {
                std::cerr << "Pa_CloseStream error: " << Pa_GetErrorText(err) << std::endl;
            }
            else {
                // We only recorded up to pAudioData->currentSampleIndex
                // Adjust maxFrameIndex so playback doesn’t exceed that
                pAudioData->totalSamplesRecorded = pAudioData->currentSampleIndex;
            }
            pAudioData->stream = nullptr;
        }
        Pa_Terminate();

        wxWindow* top = wxGetTopLevelParent(this);
        if (top)
        {
            MainWindow* mw = dynamic_cast<MainWindow*>(top);
            if (mw)
            {
                mw->playButton->button->Enable(true);
                wxCommandEvent e(myEVT_RECORD_STOPPED);
                wxPostEvent(mw, e);
            }
            else
            {
                // not actually MainWindow
                wxMessageBox("top is not a MainWindow");
            }
        }

    }
}

// Timer event handler that checks if the stream is still active
void Record_Button::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    if (!pAudioData->stream) return; // Safety check in case stream is null

    PaError err;
    int active = Pa_IsStreamActive(pAudioData->stream);
    if (active == 0)
    {
        // The callback signaled paComplete => buffer is full or done
        // Stop the timer
        m_timer.Stop();

        // Close the stream
        err = Pa_CloseStream(pAudioData->stream);
        if (err != paNoError) {
            std::cerr << "Pa_CloseStream error: "
                << Pa_GetErrorText(err) << std::endl;
        }
        pAudioData->stream = nullptr;

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
        err = Pa_CloseStream(pAudioData->stream);
        pAudioData->stream = nullptr;
        Pa_Terminate();
        button->SetBitmap(recordBundle);
        button->SetToolTip("Record");
        pStateCpy->transition(Idle);
    }
}
