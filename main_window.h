#pragma once
#include <wx/wx.h>
#include "state.h"
#include "play_button.h"
#include "record_button.h"

// forward declaration
class WavePanel;

class MainWindow : public wxFrame
{
public:
    MainWindow(const wxString& title);
    Play_Button* playButton;
    Record_Button* recordButton;
    WavePanel* wavePanel;

private:
    // We'll store references
    std::shared_ptr<State> pState;

    // We'll keep a pointer to the wave panel


    // We'll handle a "record stopped" event here
    void OnRecordStopped(wxCommandEvent& event);
    void OnDrawScreen(wxCommandEvent& event);
};