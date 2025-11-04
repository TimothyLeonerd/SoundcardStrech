// Start of Program
#include "main_window.h"
#include <wx/wx.h>

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{

    MainWindow* mainWindow = new MainWindow(wxT("Rubber Band Music"));
    mainWindow->Show(true);

    return true;
}