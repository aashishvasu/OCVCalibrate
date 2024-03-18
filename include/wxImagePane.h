#pragma once

#include <wx/wx.h>

class wxImagePane : public wxPanel
{
public:
	wxImagePane(wxFrame* parent);
	
	wxSize DoGetBestSize() const override;
	
	void paintEvent(wxPaintEvent& event);
	void paintNow();
    
	void render(wxDC& dc);

	void SetImage(wxImage& img);
	void ClrImage();
	
	void OnViewportSetup(class ViewportSetupEvent& event);

private:
	wxImage* image_ = nullptr;
	wxSize size_;
};
