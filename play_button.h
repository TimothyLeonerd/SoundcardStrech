#pragma once

#include <wx/wx.h>
#include <wx/timer.h>
#include "state.h"
#include "utils.h"

/**
 * Minimal "Play" button that can also stop playback if pressed again.
 */
class Play_Button : public wxPanel
{
public:
    Play_Button(
        wxWindow* parent,
        std::shared_ptr<State> pState,
        std::shared_ptr<AudioData> pData);

    // Event handler for the button press
    void OnPlay(wxCommandEvent& event);
    void OnRecord(wxCommandEvent& event);

    wxButton* button;

private:
    // We'll store references to the shared State and AudioData,
    // plus a PortAudio stream pointer and a timer for polling.
    std::shared_ptr<State> pStateCpy;
    std::shared_ptr<AudioData> pAudioData;

    PaStream* stream = nullptr;
    wxTimer m_timer;

    wxBitmapBundle playBundle;
    wxBitmapBundle pauseBundle;

    // We'll periodically check if playback is finished via this timer
    void OnTimer(wxTimerEvent& event);

    wxDECLARE_EVENT_TABLE();
};
