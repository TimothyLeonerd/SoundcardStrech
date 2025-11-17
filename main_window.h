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
    wxSlider* m_speedSlider = nullptr;

private:
    std::shared_ptr<State>     pState;
    std::shared_ptr<AudioData> pData;

    void OnRecordStopped(wxCommandEvent& event);
    void OnDrawScreen(wxCommandEvent& event);
    void OnSpeedSlider(wxCommandEvent& event);
};