#pragma once

#include <wx/wx.h>
#include <memory>
#include "utils.h"   // for SAMPLE, NUM_CHANNELS, etc.
#include "state.h"   // not strictly required, but you have it
                     // in your project includes
// forward-declare or include the definition of AudioData
// We'll assume "utils.h" or somewhere else defines "AudioData"

class WavePanel : public wxPanel
{
public:
    /** 
     *  Constructor: parent is a wxWindow (like a wxPanel or wxFrame) 
     *  pData is the shared_ptr that holds your recorded. 
     */
    WavePanel(wxWindow* parent, std::shared_ptr<AudioData> pData, std::shared_ptr<State> pState);

    /**
     * If you want to reassign the AudioData after creation,
     * you can add a setter like this (optional):
     *
     * void SetAudioData(std::shared_ptr<AudioData> pData) { m_pData = pData; Refresh(); }
     */

private:
    std::shared_ptr<AudioData> m_pData;
    std::shared_ptr<State>     pStateCpy;
    wxBitmap m_bmp;
    int marker_position;

    // The paint event is where we draw the waveform
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnRecordStarted(wxCommandEvent& event);
    void OnRecordStopped(wxCommandEvent& event);
    void OnPlayStarted(wxCommandEvent& event);
    void OnPlayStopped(wxCommandEvent& event);
    void InitPanelBmp();

    wxTimer m_redrawTimer;
    void OnRedrawTimer(wxTimerEvent& evt);

    wxDECLARE_EVENT_TABLE();
};
