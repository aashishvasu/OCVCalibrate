#pragma once

#include <wx/wx.h>

class FrameReadyEvent;
class ViewportSetupEvent;

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);

	// Event delegates
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnRefreshDeviceList(wxCommandEvent& event);

	void OnThreadStarted(wxCommandEvent& event);
	void OnThreadStopped(wxCommandEvent& event);

	void OnFrameReady(FrameReadyEvent& event);
	void OnViewportSetup(ViewportSetupEvent& event);

private:
	// Flow functions
	void BindEvents();
	void RefreshDeviceList();

	class CameraPanel* camPanel;
	class wxImagePane* viewport;
};
