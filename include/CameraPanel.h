#pragma once

#include "data/CameraInfo.h"
#include "data/CalibrationSettings.h"

#include <wx/wx.h>

class CameraPanel : public wxPanel
{
public:
	CameraPanel(wxWindow* parent);
	~CameraPanel();

	// Flow functions
	void BindEvents();
	void RefreshDeviceList();

	void OnDeviceChanged(wxCommandEvent& event);
	void OnStartCalibration(wxCommandEvent& event);
	void OnStopCalibration(wxCommandEvent& event);
	void OnThreadStarted(wxCommandEvent& event);
	void OnThreadSopped(wxCommandEvent& event);

protected:
	void SwitchCameraDevice(const int& ID);
	

private:
	wxChoice* comboBox;
	wxButton* calibrateBtn;
	wxButton* stopBtn;
	wxBitmapButton* settingsBtn;

	// Settings
	CameraInfo camInfo;
	CalibrationSettings calSettings;
	bool isCalibrationDataFound = false;

	// Camera threaded control class
	class CameraControl* camControl;
};
