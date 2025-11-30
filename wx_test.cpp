// Start of Program
#include "main_window.h"
#include <wx/wx.h>

// For debugging to console
#include <windows.h>
#include <cstdio>
#include <iostream>

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

// For debugging to console
void SetupConsole()
{
    AllocConsole();  // create a new console window

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);  // redirect stdout -> console
    freopen_s(&fp, "CONOUT$", "w", stderr);  // redirect stderr -> console

    // Make iostreams aware of the change
    std::ios::sync_with_stdio();
}


bool MyApp::OnInit()
{

    MainWindow* mainWindow = new MainWindow(wxT("Rubber Band Music"));

    SetupConsole(); // For debugging to console

    mainWindow->Show(true);

    return true;
}