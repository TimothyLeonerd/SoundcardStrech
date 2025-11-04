#include "main_window.h"
#include "record_button.h"
#include "play_button.h"
#include "record_button.h"
#include "utils.h"
#include "wave_panel.h"
#include "my_events.h"

MainWindow::MainWindow(const wxString& title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(500, 400))
{
    // shared pointers
    std::shared_ptr<State> pState(new State());
    std::shared_ptr<AudioData> pData(new AudioData());

    // Create a panel that will contain everything
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Create your custom buttons
    recordButton   = new Record_Button(panel, pState, pData);
    playButton     = new Play_Button(panel, pState, pData);

    // Create the wave panel
    wavePanel = new WavePanel(panel, pData);

    // Layout with sizers
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(recordButton, 0, wxALL, 5);
    buttonSizer->Add(playButton, 0, wxALL, 5);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(wavePanel, 1, wxEXPAND | wxALL, 5);
    panel->SetSizer(mainSizer);

    Centre();

    this->Bind(myEVT_RECORD_STOPPED,
        &MainWindow::OnRecordStopped,
        this);
    this->Bind(myEVT_DRAW_SCREEN,
        &MainWindow::OnDrawScreen,
        this);
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
