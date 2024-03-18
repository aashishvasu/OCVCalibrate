#include "CameraControl.h"
#include "data/CameraInfo.h"
#include "data/CalibrationSettings.h"
#include "events/CalibrateEvents.h"

#include <wx/log.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

CameraControl::CameraControl(wxWindow* parent, CalibrationSettings* settings) : parent(parent), calSettings(settings)
{
}

bool CameraControl::SwitchCameraDevice(const CameraInfo& newCamera)
{
	// will return true if the switch is successful, and that will only happen if the thread isnt running.
	if(threadRun)
		return false;

	camID = newCamera.device.ID;
	camName = newCamera.device.Name;

	// Send viewport event by temporarily getting capture data
	cv::VideoCapture cap(camID);
	const double w = cap.get(cv::CAP_PROP_FRAME_WIDTH)/2;
	const double h = cap.get(cv::CAP_PROP_FRAME_HEIGHT)/2;
	cap.release();

	SendViewportSizeEvent(wxPoint((int)w,(int)h));

	return true;
}

void CameraControl::StartThread()
{
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not create the worker thread!");
		return;
	}
	
	if (GetThread()->Run() != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not run the worker thread!");
	}
}

void CameraControl::StopThread()
{
	threadRun = false;
}

wxThread::ExitCode CameraControl::Entry()
{
	bool isSuccess = false;
	
	// Prepare chessboard data
	const int boardWidth = 9; // Number of inner corners per a chessboard row
	const int boardHeight = 6; // Number of inner corners per a chessboard column
	float squareSize = 0.025f; // Chessboard square size in meters
	cv::Size boardSize(boardWidth, boardHeight);
	
	cv::VideoCapture cap(camID); // Or use the appropriate index/URL for your camera

	if (!cap.isOpened()) {
		wxLogError("Could not open camera.");
		return (wxThread::ExitCode)1; // Exit the thread
	}

	// Set the capture resolution directly (e.g., to 640x480)
	const double w = cap.get(cv::CAP_PROP_FRAME_WIDTH)/2;
	const double h = cap.get(cv::CAP_PROP_FRAME_HEIGHT)/2;
	cap.set(cv::CAP_PROP_FRAME_WIDTH, w);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, h);

	std::vector<std::vector<cv::Point3f>> objectPoints;
	std::vector<std::vector<cv::Point2f>> imagePoints;
	std::vector<cv::Point2f> corners;
	cv::Mat frame, grayFrame;

	// Prepare object points: (0,0,0), (1,0,0), (2,0,0) ... 
	std::vector<cv::Point3f> objP;
	for (int i = 0; i < boardHeight; ++i) {
		for (int j = 0; j < boardWidth; ++j) {
			objP.push_back(cv::Point3f(j * squareSize, i * squareSize, 0));
		}
	}
	
	// Open window if needed
	// Create an OpenCV window
	//namedWindow(camName + " Calibration", cv::WINDOW_AUTOSIZE);

	// Set bool to true for run
	threadRun = true;
	
	// Send thread start event
	SendThreadStartedEvent();

	// We will collect 20 successful frames for calibration data
	const int maxFrames = 20;
	const int maxMultiplier = 4;
	int successFrames = 0;
	while(successFrames < maxFrames*maxMultiplier && threadRun)
	{
		// Start measuring tick
		const double start_time = (double)cv::getTickCount();
		
		cap >> frame;
		if (frame.empty())
			break;

		cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
		const bool found = findChessboardCorners(grayFrame, boardSize, corners, 
					cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);

		drawChessboardCorners(frame, boardSize, corners, found);

		if (found)
		{
			successFrames++;
			if(successFrames % maxMultiplier == 0)
			{
				cornerSubPix(grayFrame, corners, cv::Size(11, 11), cv::Size(-1, -1),
							 cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.1));
				imagePoints.push_back(corners);
				objectPoints.push_back(objP);
			}
		}
		
		//imshow(camName + " Calibration", frame);
		

		// Calculate tick in ms and send
		const double end_time = (double)cv::getTickCount();
		const double elapsed_time = (end_time - start_time) / cv::getTickFrequency() * 1000;

		SendFrameReadyEvent(wxFromCvMat(frame), (int)elapsed_time);
		
		if (cv::waitKey(30) >= 0)
		{
			StopThread();
			break;
		}
	}

	// Destroy calibration window
	//cv::destroyWindow(camName + " Calibration"); // Close OpenCV window

	
	if(successFrames == 0)
	{
		wxLogError("No calibration data generated");
		isSuccess = false;
	}
	else
	{
		// Calibration
		cv::Mat cameraMatrix, distCoeffs;
		std::vector<cv::Mat> rvecs, tvecs;
		calibrateCamera(objectPoints, imagePoints, grayFrame.size(), cameraMatrix, distCoeffs, rvecs, tvecs);

		calSettings->GetData()["calibration"]["date"] = wxDateTime::Now().FormatDate();
		calSettings->GetData()["calibration"]["cameraMatrix"] = CalibrationSettings::MatToString(cameraMatrix);
		calSettings->GetData()["calibration"]["distCoeffs"] = CalibrationSettings::MatToString(distCoeffs);
		
		if(calSettings->Save(camID))
		{
			wxLogMessage("Calibration data saved");
			isSuccess = true;
		}
		else
		{
			wxLogError("Calibration data generated but not saved");
			isSuccess = false;
		}
	}

	cap.release();

	// Send thread end
	SendThreadStoppedEvent();
	
	return (wxThread::ExitCode)!isSuccess;
}

void CameraControl::SendViewportSizeEvent(const wxPoint& point)
{
	ViewportSetupEvent event(calEVT_VIEWPORT_DETAILS, calID_VIEWPORT);

	event.SetViewportSize(point);

	parent->ProcessWindowEvent(event);
}

void CameraControl::SendThreadStartedEvent()
{
	wxCommandEvent event(calEVT_THREAD_STARTED, calID_THREADSTARTED);
	
	event.SetString("Camera Calibration Thread Started");
	
	parent->ProcessWindowEvent(event);
}

void CameraControl::SendThreadStoppedEvent()
{	
	wxCommandEvent event(calEVT_THREAD_STOPPED, calID_THREADSTOPPED);
	
	event.SetString("Camera Calibration Thread Stopped");
	
	parent->ProcessWindowEvent(event);
}

void CameraControl::SendFrameReadyEvent(const wxImage& image, const int& milliseconds)
{
	FrameReadyEvent event(calEVT_VIEWPORT_DETAILS, calID_FRAMEREADY);
	event.SetImage(image);
	event.SetInt(milliseconds);
	parent->ProcessWindowEvent(event);
}

wxImage CameraControl::wxFromCvMat(const cv::Mat& mat)
{
	CV_Assert(mat.depth() == CV_8U); // Check for valid depth
	cv::Mat tempMat;
	cvtColor(mat, tempMat, cv::COLOR_BGR2RGB); // Convert from BGR to RGB

	// Create a new data array and copy the image data into it
	unsigned char* data = new unsigned char[tempMat.rows * tempMat.cols * tempMat.channels()];
	std::memcpy(data, tempMat.data, tempMat.rows * tempMat.cols * tempMat.channels());

	return {tempMat.cols, tempMat.rows, data, true}; // true to indicate that the data should not be deallocated by wxImage
}

