#pragma once
#include <wx/event.h>

// Declare custom events
wxDECLARE_EVENT(myEVT_RECORD_STARTED, wxCommandEvent);
wxDECLARE_EVENT(myEVT_RECORD_STOPPED, wxCommandEvent);
wxDECLARE_EVENT(myEVT_DRAW_SCREEN, wxCommandEvent);

// Define custom id
const int MY_ID_RECORD_STARTED = wxID_HIGHEST + 998;
const int MY_ID_RECORD_STOPPED = wxID_HIGHEST + 999;
const int MY_ID_DRAW_SCREEN = wxID_HIGHEST + 1000;