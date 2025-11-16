#include "wave_panel.h"
#include <algorithm>  // for std::min, etc.

// Macro for event table
wxBEGIN_EVENT_TABLE(WavePanel, wxPanel)
EVT_PAINT(WavePanel::OnPaint)
//EVT_SIZE(WavePanel::OnSize)
wxEND_EVENT_TABLE()

WavePanel::WavePanel(wxWindow* parent, std::shared_ptr<AudioData> pData)
    : wxPanel(parent, wxID_ANY), m_pData(pData)
{
    // Nothing special in constructor right now.
    // If you want to do double-buffering or set background style:
    // SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void WavePanel::OnSize(wxSizeEvent& event) {
    // Gets called when size changes. Dummy for the time being
}

// The main drawing routine, called whenever wxWidgets must refresh the panel
void WavePanel::OnPaint(wxPaintEvent& event)
{   
    // 1) Get current panel size
    int width, height;
    GetClientSize(&width, &height);

    wxBitmap bmp;
    bmp.CreateWithDIPSize(GetClientSize(), GetDPIScaleFactor());
    {
        wxMemoryDC memdc(bmp);

        // Check if we have audio data
        if (!m_pData || !m_pData->recorded) {
            memdc.SetBrush(*wxWHITE_BRUSH);
            memdc.SetPen(*wxWHITE_PEN);
            memdc.DrawRectangle(0, 0, width, height);

            // No data yet, just draw some text or do nothing
            memdc.SetTextForeground(*wxBLACK);
            memdc.DrawText("No audio recorded.", 10, 10);
            return;
        }

        // We want to display only the *recorded* portion.
        // e.g., currentSampleIndex is min(currentSampleIndex, maxFrameIndex).
        // (If user recorded 2 seconds of a 10 second buffer, currentSampleIndex=somevalue,
        //  or if they've used the "stop early" logic, maxFrameIndex might
        //  be set to currentSampleIndex after stopping.)
        int currentSampleIndex = std::min(m_pData->currentSampleIndex, m_pData->maxFrameIndex);

        if (currentSampleIndex <= 0) {
            // There's a buffer, but 0 frames used
            memdc.DrawText("No audio recorded yet.", 10, 10);
            return;
        }
        else if (m_pData->lastSampleIndex == 0) {
            memdc.SetBrush(*wxWHITE_BRUSH);
            memdc.SetPen(*wxWHITE_PEN);
            memdc.DrawRectangle(0, 0, width, height);
        }

        // 4) We'll draw the first channel only
        SAMPLE* samples = m_pData->recorded;  // pointer to array
        // Center line for amplitude=0 is halfway down the panel
        float midY = height / 2.0f;

        float lastX = (float)m_pData->lastSampleIndex * (width / (float)m_pData->maxFrameIndex);
        float lastY = midY;

        // Erase previous position marker
        memdc.SetPen(*wxWHITE_PEN);
        memdc.DrawLine((int)lastX + 1, (int)0, (int)lastX + 1, (int)height);

        memdc.SetBrush(*wxWHITE_BRUSH);
        memdc.SetPen(*wxWHITE_PEN);
        memdc.DrawRectangle(0, 0, width, height);

        for (int i = 0; i < currentSampleIndex; i++)
        {
            // We only read the first channel
            float sampleVal = samples[i * NUM_CHANNELS];

            // Map sampleVal from -1..+1 to vertical range
            float y = midY - sampleVal * (height / 2.0f);

            // Map i from 0..currentSampleIndex to 0..width
            float x = (float)i * (width / (float)m_pData->maxFrameIndex);

            // Draw new wav form
            memdc.SetPen(*wxBLACK_PEN);
            memdc.DrawLine((int)lastX, (int)lastY, (int)x, (int)y);

            lastX = x;
            lastY = y;
        }

#if 1
        // Draw new position marker
        memdc.SetPen(*wxBLUE_PEN);
        memdc.DrawLine((int)lastX + 1, (int)0, (int)lastX + 1, (int)height);
#endif

        m_pData->lastSampleIndex = currentSampleIndex;
    }

    wxPaintDC dc(this);
    dc.DrawBitmap(bmp, wxPoint(0, 0));
}
