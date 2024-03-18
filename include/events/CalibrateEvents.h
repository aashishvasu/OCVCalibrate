#pragma once

#include <wx/event.h>
#include <wx/image.h>

// Events setup
class ViewportSetupEvent;
class FrameReadyEvent;
wxDECLARE_EVENT(calEVT_VIEWPORT_DETAILS, ViewportSetupEvent);
wxDECLARE_EVENT(calEVT_FRAME_READY, FrameReadyEvent);
// Command events
wxDECLARE_EVENT(calEVT_THREAD_STARTED, wxCommandEvent);
wxDECLARE_EVENT(calEVT_THREAD_STOPPED, wxCommandEvent);

enum CalibrateEventEnum
{
	calID_VIEWPORT = 0,
	calID_FRAMEREADY,
	calID_THREADSTARTED,
	calID_THREADSTOPPED
};

// Custom viewport class definition
class ViewportSetupEvent: public wxCommandEvent
{
public:
	ViewportSetupEvent(wxEventType commandType = calEVT_VIEWPORT_DETAILS, int id = 0) : wxCommandEvent(commandType, id) { }
	ViewportSetupEvent(const ViewportSetupEvent& event) : wxCommandEvent(event) { this->SetViewportSize(event.GetViewportSize()); }

	wxEvent* Clone() const override { return new ViewportSetupEvent(*this); }

	// Set get
	void SetViewportSize(const wxPoint& p){ _viewportSize=p; }
	wxPoint GetViewportSize() const { return _viewportSize; }
	
private:
	wxPoint _viewportSize;
	
};

// Custom frame ready event
class FrameReadyEvent: public wxCommandEvent
{
public:
	FrameReadyEvent(wxEventType commandType = calEVT_FRAME_READY, int id = 0) : wxCommandEvent(commandType, id) { }
	FrameReadyEvent(const FrameReadyEvent& event) : wxCommandEvent(event) { this->SetImage(event.GetImage()); }

	wxEvent* Clone() const override { return new FrameReadyEvent(*this); }

	void SetImage(const wxImage& image) { this->_image = image; }
	wxImage GetImage() const { return _image; }

private:
	wxImage _image;
};

