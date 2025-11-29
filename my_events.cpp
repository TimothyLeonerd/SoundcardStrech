#include "my_events.h"

// events
wxDEFINE_EVENT(myEVT_RECORD_STARTED, wxCommandEvent);
wxDEFINE_EVENT(myEVT_RECORD_STOPPED, wxCommandEvent);
wxDEFINE_EVENT(myEVT_DRAW_SCREEN, wxCommandEvent);
wxDEFINE_EVENT(myEVT_PLAY_STARTED, wxCommandEvent);
wxDEFINE_EVENT(myEVT_PLAY_STOPPED, wxCommandEvent);