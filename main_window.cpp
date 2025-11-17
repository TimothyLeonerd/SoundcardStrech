#include "main_window.h"
#include "record_button.h"
#include "play_button.h"
#include "utils.h"
#include "wave_panel.h"
#include "my_events.h"

MainWindow::MainWindow(const wxString& title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(500, 400))
{
    // shared pointers
    pState = std::make_shared<State>();
    pData = std::make_shared<AudioData>();

    // Create a panel that will contain everything
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Create your custom buttons
    recordButton   = new Record_Button(panel, pState, pData);
    playButton     = new Play_Button(panel, pState, pData);

    // Create the wave panel
    wavePanel = new WavePanel(panel, pData);
    
    m_speedSlider = new wxSlider(
        panel,
        wxID_ANY,
        1000,   // initial -> 1.0x
        500,    // min -> 0.5x
        2000,   // max -> 2.0x
        wxDefaultPosition,
        wxSize(150, -1),
        wxSL_HORIZONTAL
    );

    m_speedSlider->SetToolTip("Playback speed (0.5x – 2.0x)");
    
    // Layout with sizers


    wxBoxSizer* controlSizer = new wxBoxSizer(wxHORIZONTAL);
    controlSizer->Add(recordButton, 0, wxALL, 5);
    controlSizer->Add(playButton, 0, wxALL, 5);
    controlSizer->Add(m_speedSlider, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);


    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(controlSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(wavePanel, 1, wxEXPAND | wxALL, 5);
    panel->SetSizer(mainSizer);

    Centre();

    this->Bind(myEVT_RECORD_STOPPED,
        &MainWindow::OnRecordStopped,
        this);
    this->Bind(myEVT_DRAW_SCREEN,
        &MainWindow::OnDrawScreen,
        this);

    this->Bind(wxEVT_SLIDER, &MainWindow::OnSpeedSlider, this);
}

// Called every time onTimer is called
void MainWindow::OnDrawScreen(wxCommandEvent& event)
{
    if (wavePanel)
    {
        wavePanel->Refresh(false);
    }
}

// Called when Record_Button posts the "stop" event
void MainWindow::OnRecordStopped(wxCommandEvent& event)
{
    // Just refresh the wave panel
    // So it redraws and displays the newly recorded data
    //wxMessageBox("OnRecordStopped was called!", "Debug");

    if (wavePanel)
    {
        wavePanel->Refresh(false);
    }
}

void MainWindow::OnSpeedSlider(wxCommandEvent& WXUNUSED(event))
{
    if (!m_speedSlider || !pState) return;

    int value = m_speedSlider->GetValue();  // 500..2000
    double ratio = 1000.0 / static_cast<double>(value);  // inverse of speed

    pState->timeRatio = ratio;
    // or pState->SetTimeRatio(ratio);

    // Optional: keep tooltip synced
    wxString tip;
    tip.Printf("Playback speed: %.2fx", ratio);
    m_speedSlider->SetToolTip(tip);
}