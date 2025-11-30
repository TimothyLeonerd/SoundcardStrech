#pragma once
#include "state.h"
#include "utils.h"
#include <wx/timer.h>
#include <wx/wx.h>

class Record_Button : public wxPanel
{
public:
    Record_Button(wxWindow* parent,
        std::shared_ptr<State> pState,
        std::shared_ptr<AudioData> pData);

    void OnRecord(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);           // <-- We'll poll Pa_IsStreamActive

    wxButton* button;

private:
    std::shared_ptr<State> pStateCpy;
    std::shared_ptr<AudioData> pAudioData;

    // We'll use a wxTimer to periodically check if the stream is done recording
    wxTimer m_timer;

    wxBitmapBundle recordBundle;
    wxBitmapBundle stopBundle;

    // Needed for wxWidgets event routing
    wxDECLARE_EVENT_TABLE();

    // helper functions
    void updateGuiRecordStarted();
};
