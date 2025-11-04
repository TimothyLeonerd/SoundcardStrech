#include "wave_panel.h"
#include <algorithm>  // for std::min, etc.

// Macro for event table
wxBEGIN_EVENT_TABLE(WavePanel, wxPanel)
EVT_PAINT(WavePanel::OnPaint)
wxEND_EVENT_TABLE()

WavePanel::WavePanel(wxWindow* parent, std::shared_ptr<AudioData> pData)
    : wxPanel(parent, wxID_ANY), m_pData(pData)
{
    // Nothing special in constructor right now.
    // If you want to do double-buffering or set background style:
    // SetBackgroundStyle(wxBG_STYLE_PAINT);
}

// The main drawing routine, called whenever wxWidgets must refresh the panel
void WavePanel::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    // 1) Get current panel size
    int width, height;
    GetClientSize(&width, &height);

    // Check if we have audio data
    if (!m_pData || !m_pData->recorded) {
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawRectangle(0, 0, width, height);

        // No data yet, just draw some text or do nothing
        dc.SetTextForeground(*wxBLACK);
        dc.DrawText("No audio recorded.", 10, 10);
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
        dc.DrawText("No audio recorded yet.", 10, 10);
        return;
    }
    else if (m_pData->lastSampleIndex == 0){
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawRectangle(0, 0, width, height);
    }

    // 4) We'll draw the first channel only
    SAMPLE* samples = m_pData->recorded;  // pointer to array
    // Center line for amplitude=0 is halfway down the panel
    float midY = height / 2.0f;

    float lastX = (float)m_pData->lastSampleIndex * (width / (float)m_pData->maxFrameIndex);
    float lastY = midY;

    for (int i = m_pData->lastSampleIndex; i < currentSampleIndex; i++)
    {
        // We only read the first channel
        float sampleVal = samples[i * NUM_CHANNELS];

        // Map sampleVal from -1..+1 to vertical range
        float y = midY - sampleVal * (height / 2.0f);

        // Map i from 0..currentSampleIndex to 0..width
        float x = (float)i * (width / (float)m_pData->maxFrameIndex);

        // Erase previous position marker
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawLine((int)lastX+1, (int)0, (int)lastX+1, (int)height);

        // Draw new wav form
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine((int)lastX, (int)lastY, (int)x, (int)y);

        // Draw new position marker
        dc.SetPen(*wxBLUE_PEN);
        dc.DrawLine((int)x+1, (int)0, (int)x+1, (int)height);

        lastX = x;
        lastY = y;
    }

    m_pData->lastSampleIndex = currentSampleIndex;
}
