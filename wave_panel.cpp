#include "wave_panel.h"
#include "my_events.h"
#include <algorithm>  // for std::min, etc.

namespace {
    constexpr int ID_REDRAW_TIMER = wxID_HIGHEST + 101;
}

// Macro for event table
wxBEGIN_EVENT_TABLE(WavePanel, wxPanel)
EVT_PAINT(WavePanel::OnPaint)
EVT_SIZE(WavePanel::OnSize)
EVT_TIMER(ID_REDRAW_TIMER, WavePanel::OnRedrawTimer)
EVT_COMMAND(wxID_ANY, myEVT_RECORD_STARTED, WavePanel::OnRecordStarted)
EVT_COMMAND(wxID_ANY, myEVT_RECORD_STOPPED, WavePanel::OnRecordStopped)
EVT_COMMAND(wxID_ANY, myEVT_PLAY_STARTED, WavePanel::OnPlayStarted)
EVT_COMMAND(wxID_ANY, myEVT_PLAY_STOPPED, WavePanel::OnPlayStopped)
wxEND_EVENT_TABLE()

WavePanel::WavePanel(wxWindow* parent, std::shared_ptr<AudioData> pData, std::shared_ptr<State> pState)
    : wxPanel(parent, wxID_ANY), m_pData(pData), pStateCpy(pState), m_redrawTimer(this, ID_REDRAW_TIMER)
{
    // Nothing special in constructor right now.
    // If you want to do double-buffering or set background style:
    // SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void WavePanel::OnSize(wxSizeEvent& event)
{
    InitPanelBmp();

    m_pData->lastSampleIndex = 0;

    // Trigger repaint with new size
    Refresh(false);
    event.Skip();  // let wxWidgets handle default behaviour as well
}

void WavePanel::OnRecordStarted(wxCommandEvent& event)
{
    InitPanelBmp();

    if (m_pData) {
        m_pData->lastSampleIndex = 0;
    }

    marker_position = -1;

    m_redrawTimer.Start(33);
    Refresh(false);
}

void WavePanel::OnRecordStopped(wxCommandEvent& event)
{
    m_redrawTimer.Stop();
    Refresh(false);            // final repaint if needed
}

void WavePanel::OnPlayStarted(wxCommandEvent& event)
{
    if (!m_pData || !m_pData->recorded)
        return;

    marker_position = -1;

    m_redrawTimer.Start(33);
    Refresh(false);
}

void WavePanel::OnPlayStopped(wxCommandEvent& event)
{
    m_redrawTimer.Stop();
    Refresh(false);  // one last update (e.g. to leave marker at end or clear it)
}

void WavePanel::InitPanelBmp()
{
    const wxSize size = GetClientSize();
    if (size.x <= 0 || size.y <= 0)
        return;

    // Create bitmap with correct DPI
    m_bmp.CreateWithDIPSize(size, GetDPIScaleFactor());

    // Make panel white again
    wxMemoryDC memdc(m_bmp);
    memdc.SetBrush(*wxWHITE_BRUSH);
    memdc.SetPen(*wxWHITE_PEN);
    memdc.DrawRectangle(0, 0, size.x, size.y);

    marker_position = -1;
}

// The main drawing routine, called whenever wxWidgets must refresh the panel
void WavePanel::OnPaint(wxPaintEvent& event)
{   
    wxPaintDC dc(this);

    int width, height;
    GetClientSize(&width, &height);

    if (!m_bmp.IsOk())
        InitPanelBmp();

    {
        wxMemoryDC memdc(m_bmp);

        // If no audio data
        if (!m_pData || !m_pData->recorded)
        {
            memdc.SetBrush(*wxWHITE_BRUSH);
            memdc.SetPen(*wxWHITE_PEN);
            memdc.DrawRectangle(0, 0, width, height);

            memdc.SetTextForeground(*wxBLACK);
            memdc.DrawText("No audio recorded.", 10, 10);
        }
        else if (pStateCpy->state == Recording)
        {
            int currentSampleIndex = std::min(m_pData->currentSampleIndex, m_pData->maxSamplesBuffer);

            SAMPLE* samples = m_pData->recorded;
            const float midY = height / 2.0f;

            float lastX = (float)m_pData->lastSampleIndex * (width / (float)m_pData->maxSamplesBuffer);
            float lastY = midY;

            // Erase previous marker
            if (marker_position >= 0) {
                memdc.SetPen(*wxWHITE_PEN);
                memdc.DrawLine(marker_position, 0, marker_position, height);
            }

            // draw waveform
            memdc.SetPen(*wxBLACK_PEN);
            for (int i = m_pData->lastSampleIndex; i < currentSampleIndex; ++i)
            {
                float sampleVal = samples[i * NUM_CHANNELS];
                float y = midY - sampleVal * (height / 2.0f);
                float x = (float)i * (width / (float)m_pData->maxSamplesBuffer);

                memdc.DrawLine((int)lastX, (int)lastY, (int)x, (int)y);
                lastX = x;
                lastY = y;
            }

            // draw new position marker
            marker_position = lastX + 1;
            memdc.SetPen(*wxBLUE_PEN);
            memdc.DrawLine(marker_position, 0, marker_position, height);

            m_pData->lastSampleIndex = currentSampleIndex;
        }
        else if (pStateCpy->state == Playing) {
            int currentSampleIndex = std::min(m_pData->currentSampleIndex, m_pData->maxSamplesBuffer);

            SAMPLE* samples = m_pData->recorded;
            const float midY = height / 2.0f;

            float lastX = (float)m_pData->lastSampleIndex * (width / (float)m_pData->maxSamplesBuffer);
            float lastY = midY;

            // Erase previous marker
            if (marker_position >= 0) {
                memdc.SetPen(*wxWHITE_PEN);
                memdc.DrawLine(marker_position, 0, marker_position, height);
            }

            // draw new position marker
            marker_position = lastX + 1;
            memdc.SetPen(*wxBLUE_PEN);
            memdc.DrawLine(marker_position, 0, marker_position, height);

            m_pData->lastSampleIndex = currentSampleIndex;
        }
    }

    // Draw buffer on screen
    dc.DrawBitmap(m_bmp, 0, 0, false);
}

void WavePanel::OnRedrawTimer(wxTimerEvent&)
{
    if (pStateCpy->state == Recording || pStateCpy->state == Playing)
        Refresh(false);
    else
        m_redrawTimer.Stop();
}