#include "MainFrame.h"
#include "CameraPanel.h"
#include "wxImagePane.h"
#include "data/CameraInfo.h"
#include "events/CalibrateEvents.h"
#include "utils/DeviceEnumerator.h"

#include <unordered_map>

constexpr int ID_REFRESH_DEVICES = 1000;

// Custom event defines
wxDEFINE_EVENT(calEVT_FRAME_READY, FrameReadyEvent);

MainFrame::MainFrame(const wxString& title)	: wxFrame(nullptr, wxID_ANY, title)
{
	SetBackgroundColour(wxNullColour);

	// Main Menu
	// Create menu
	wxMenuBar* menuBar = new wxMenuBar();
	// File menu
	wxMenu* fileMenu = new wxMenu();
	fileMenu->Append(ID_REFRESH_DEVICES, _T("&Refresh Device List"));
	fileMenu->Append(wxID_EXIT, _T("&Quit"));
	menuBar->Append(fileMenu, _T("&File"));
	// About menu
	wxMenu* helpMenu = new wxMenu();
	helpMenu->Append(wxID_ABOUT, _T("&About"));
	menuBar->Append(helpMenu, _T("&Help"));
	SetMenuBar(menuBar);

	// Camera panel
	camPanel = new CameraPanel(this);

	// Viewport
	viewport = new wxImagePane(this);

	// Box sizer
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);
	SetAutoLayout(true);

	// Add sizer elements
	mainSizer->Add(camPanel, 0, wxEXPAND|wxALL, 5);
	mainSizer->Add(viewport, 1, wxEXPAND, 5);

	// Create status bar
	CreateStatusBar();
	GetStatusBar()->SetStatusText("Ready");

	// Bind events
	BindEvents();
}

void MainFrame::OnExit(wxCommandEvent& event)
{
	Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
	// Show about dialog
	wxDialog* dialog = new wxDialog(this, wxID_ABOUT, "About");
	dialog->ShowModal();
}

void MainFrame::OnRefreshDeviceList(wxCommandEvent& event)
{
	RefreshDeviceList();

	wxLogMessage("Refreshed device list");
}

void MainFrame::OnThreadStarted(wxCommandEvent& event)
{
	// Resize window to fit
	Fit();
}

void MainFrame::OnThreadStopped(wxCommandEvent& event)
{
	GetStatusBar()->SetStatusText("Ready");

	// Clear image in viewport area
	viewport->ClrImage();

	// Resize window to fit
	Fit();
}

void MainFrame::OnFrameReady(FrameReadyEvent& event)
{
	const std::string text = std::to_string(event.GetInt())+" ms";
	GetStatusBar()->SetStatusText(text);

	// Set image in viewport
	viewport->SetImage(event.GetImage());
}

void MainFrame::OnViewportSetup(ViewportSetupEvent& event)
{
	// Resize window to fit
	Fit();

	event.Skip();
}

void MainFrame::BindEvents()
{
	// Bind events directly to event handler functions
	Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
	Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);

	Bind(calEVT_THREAD_STARTED, &MainFrame::OnThreadStarted, this);
	Bind(calEVT_THREAD_STOPPED, &MainFrame::OnThreadStopped, this);

	Bind(calEVT_FRAME_READY, &MainFrame::OnFrameReady, this);
	Bind(calEVT_VIEWPORT_DETAILS, &MainFrame::OnViewportSetup, this);

	Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnRefreshDeviceList, this, ID_REFRESH_DEVICES);
}

void MainFrame::RefreshDeviceList()
{
	// Load device list
	std::unordered_map<int, CameraHWDevice> devices;
	ListVideoCaptureDevices(devices);
	CameraInfo::SetDeviceList(devices);

	camPanel->RefreshDeviceList();
}
