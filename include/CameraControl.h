#pragma once

#include <wx/window.h>
#include <wx/thread.h>

namespace cv
{
	class Mat;
}

class CameraControl : wxThreadHelper
{
public:
	CameraControl(wxWindow* parent, class CalibrationSettings* settings);
	~CameraControl() override { calSettings = nullptr; }

	bool SwitchCameraDevice(const struct CameraInfo& newCamera);

	void StartThread();
	void StopThread();
	
protected:
	wxThread::ExitCode Entry() override;

	void SendViewportSizeEvent(const wxPoint& point);
	void SendThreadStartedEvent();
	void SendThreadStoppedEvent();
	void SendFrameReadyEvent(const wxImage& image, const int& milliseconds);

	static wxImage wxFromCvMat(const cv::Mat& mat);
private:
	bool threadRun = false;
	int camID = 0;
	std::string camName;
	wxWindow* parent;

	class CalibrationSettings* calSettings;
};
