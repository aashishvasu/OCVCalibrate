#include "App.h"
#include "MainFrame.h"
#include "utils/AppInfo.h"
#include "utils/DeviceEnumerator.h"

#include <unordered_map>

wxIMPLEMENT_APP(CalibrateApp);

bool CalibrateApp::OnInit()
{
	// Load device list
	std::unordered_map<int, CameraHWDevice> devices;
	ListVideoCaptureDevices(devices);
	CameraInfo::SetDeviceList(devices);
	
	// make sure to call this first
	wxInitAllImageHandlers();
	
	MainFrame* frame = new MainFrame(wxString::Format("OCVCalibrate [%s]", GenerateSemanticVersion()));
	frame->SetClientSize(800, 600);
	//frame->Fit();
	frame->Show(true);
	
	return wxApp::OnInit();
}
