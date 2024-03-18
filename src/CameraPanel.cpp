#include "CameraPanel.h"
#include "CameraControl.h"
#include "data/CameraInfo.h"
#include "data/CalibrationSettings.h"
#include "events/CalibrateEvents.h"

#include <wx/artprov.h>
#include <wx/stdpaths.h>

// Event defines
wxDEFINE_EVENT(calEVT_THREAD_STARTED, wxCommandEvent);
wxDEFINE_EVENT(calEVT_THREAD_STOPPED, wxCommandEvent);

CameraPanel::CameraPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME)
{	
	// Box sizer
	wxBoxSizer* panelSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(panelSizer);

	// Create combo box for device choices
	comboBox = new wxChoice(this, wxID_ANY);
	
	// Init class data
	camControl = new CameraControl(this, &calSettings);
	RefreshDeviceList();
	SwitchCameraDevice(0);

	// Calibrate button
	calibrateBtn = new wxButton(this, wxID_ANY, "Calibrate");

	// Stop button
	stopBtn = new wxButton(this, wxID_ANY, "Stop");
	stopBtn->Hide();

	// Settings button
	const wxBitmap settingsIcn = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_BUTTON);
	settingsBtn = new wxBitmapButton(this, wxID_ANY, settingsIcn);

	// Add sizer elements
	panelSizer->Add(comboBox, 0, wxALIGN_CENTRE|wxALL, 5);
	panelSizer->AddStretchSpacer();
	panelSizer->Add(calibrateBtn, 0, wxALL, 5);
	panelSizer->Add(stopBtn, 0, wxALL, 5);
	panelSizer->Add(settingsBtn, 0, wxALL, 5);

	BindEvents();
}

CameraPanel::~CameraPanel()
{
	delete camControl;
}

void CameraPanel::BindEvents()
{
	calibrateBtn->Bind(wxEVT_BUTTON, &CameraPanel::OnStartCalibration, this);
	stopBtn->Bind(wxEVT_BUTTON, &CameraPanel::OnStopCalibration, this);
	comboBox->Bind(wxEVT_CHOICE, &CameraPanel::OnDeviceChanged, this);

	Bind(calEVT_THREAD_STARTED, &CameraPanel::OnThreadStarted, this);
	Bind(calEVT_THREAD_STOPPED, &CameraPanel::OnThreadSopped, this);
}

void CameraPanel::RefreshDeviceList()
{
	// Clear and repopulate choices
	comboBox->Clear();
	
	for(std::pair<const int, CameraHWDevice>& it : CameraInfo::GetDeviceList())
	{
		comboBox->Append(it.second.Name);
	}
	comboBox->SetSelection(0);
}

void CameraPanel::OnDeviceChanged(wxCommandEvent& event)
{
	SwitchCameraDevice(comboBox->GetSelection());
}

void CameraPanel::OnStartCalibration(wxCommandEvent& event)
{
	comboBox->Disable();
	calibrateBtn->Disable();
	settingsBtn->Disable();

	camControl->StartThread();
}

void CameraPanel::OnStopCalibration(wxCommandEvent& event)
{
	camControl->StopThread();
}

void CameraPanel::OnThreadStarted(wxCommandEvent& event)
{
	comboBox->Disable();
	stopBtn->Show();
	GetSizer()->Layout();
	calibrateBtn->Disable();
	settingsBtn->Disable();

	// Propagate event upwards
	event.Skip();
}

void CameraPanel::OnThreadSopped(wxCommandEvent& event)
{
	comboBox->Enable();
	stopBtn->Hide();
	GetSizer()->Layout();
	calibrateBtn->Enable();
	settingsBtn->Enable();

	// Propagate event upwards
	event.Skip();
}

void CameraPanel::SwitchCameraDevice(const int& ID)
{
	// Check if calibration folder exists. if not, create it
	if(!Config::CheckIfDirExists("calibration"))
		Config::MakeDir("calibration");

	if(calSettings.Load(ID))
	{
		// If the calibration file exists, check the date field to see if data has been generated
		const std::string runData = calSettings.GetData().get("calibration").get("date");
		isCalibrationDataFound = !runData.empty();
	}
	else
	{
		calSettings.Save(ID, true);
		isCalibrationDataFound = false;
	}

	camInfo.device = CameraInfo::GetDeviceList()[ID];

	// Switch device in camera control
	if(!camControl)
		return;

	camControl->SwitchCameraDevice(camInfo);
}
