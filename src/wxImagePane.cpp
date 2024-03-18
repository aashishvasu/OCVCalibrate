#include "wxImagePane.h"
#include "events/CalibrateEvents.h"

wxDEFINE_EVENT(calEVT_VIEWPORT_DETAILS, ViewportSetupEvent);

wxImagePane::wxImagePane(wxFrame* parent) : wxPanel(parent)
{
	// Bind events
	Bind(wxEVT_PAINT, &wxImagePane::paintEvent, this);
	Bind(calEVT_VIEWPORT_DETAILS, &wxImagePane::OnViewportSetup, this);
}

wxSize wxImagePane::DoGetBestSize() const
{
	if(size_.IsFullySpecified())
	{
		// Return the size of the image
		return size_;
	}
	else
	{
		// If there's no image, fall back to the default implementation
		return wxPanel::DoGetBestSize();
	}
}


void wxImagePane::paintEvent(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	render(dc);
}

void wxImagePane::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void wxImagePane::render(wxDC& dc)
{
	if(image_)
	{
		SetSize(dc.GetSize());
		const wxBitmap resized = wxBitmap(*image_);
		dc.DrawBitmap(resized, 0, 0, false);
	}
}

void wxImagePane::SetImage(wxImage& img)
{
	image_ = &img;
	InvalidateBestSize();
	Refresh();
	Update();
}

void wxImagePane::ClrImage()
{
	image_ = nullptr;
	InvalidateBestSize();
	Refresh();
	Update();
}

void wxImagePane::OnViewportSetup(ViewportSetupEvent& event)
{
	size_ = wxSize(event.GetViewportSize().x, event.GetViewportSize().y);
	SetSize(size_);
	InvalidateBestSize();
	
	event.Skip();
}

