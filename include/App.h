#pragma once

#include <wx/wx.h>

class CalibrateApp : public wxApp {
public:
	virtual bool OnInit() override;
	void ResizeToFit();
};