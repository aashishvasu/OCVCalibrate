#pragma once

#include "data/CameraInfo.h"

#include <string>
#include <unordered_map>
#include <wx/wx.h>
#ifdef _WIN32
	// Include DirectShow headers for Windows
	#include <dshow.h>
	#pragma comment(lib, "strmiids.lib")
#else
	// Include V4L2 headers for Linux
	#include <fcntl.h>
	#include <linux/videodev2.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
#endif

// Declare platform-specific functions
#ifdef _WIN32
inline bool ListVideoCaptureDevicesWindows(std::unordered_map<int, CameraHWDevice>& Devices)
{
	// Enumerate DirectShow devices on Windows
	ICreateDevEnum *devEnum = NULL;
	CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&devEnum));
	if (FAILED(hr)) {
		wxLogError("Failed to create device enumerator.");
		CoUninitialize();
		return false;
	}

	IEnumMoniker *enumMoniker = NULL;
	hr = devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
	if (hr != S_OK) {
		wxLogMessage("No video capture devices found.");
		devEnum->Release();
		CoUninitialize();
		return false;
	}

	IMoniker *moniker = NULL;
	ULONG fetched;
	int counter = 0;
	while (enumMoniker->Next(1, &moniker, &fetched) == S_OK) {
		IPropertyBag *propBag;
		moniker->BindToStorage(0, 0, IID_PPV_ARGS(&propBag));
		VARIANT var;
		VariantInit(&var);
		propBag->Read(L"FriendlyName", &var, 0); // Assuming you only need the FriendlyName

		// Convert WCHAR* (UTF-16) to std::string (UTF-8)
		char friendlyName[256]; // Adjust size as needed
		WideCharToMultiByte(CP_UTF8, 0, var.bstrVal, -1, friendlyName, sizeof(friendlyName), NULL, NULL);
		VariantClear(&var);
        
		// Add to devices list
		Devices[counter] = CameraHWDevice(counter, std::string(friendlyName));
		counter++;
        
		propBag->Release();
		moniker->Release();
	}
	enumMoniker->Release();
	devEnum->Release();
	CoUninitialize();
	return true;
}

//bool ListVideoCaptureDevicesMacOS(std::unordered_map<int, CameraHWDevice>& Devices);

#else
inline bool ListVideoCaptureDevicesLinux(std::unordered_map<int, CameraHWDevice>& Devices)
{
	bool deviceFound = false;
	for (int i = 0; i < 64; ++i) { // Arbitrary limit; adjust as needed
		std::string devicePath = "/dev/video" + std::to_string(i);

		// Attempt to open the device
		int fd = open(devicePath.c_str(), O_RDWR);
		if (fd == -1) {
			// Log that we couldn't open this device, might not exist
			wxLogMessage(wxString::Format("Could not open device: %s, assuming it doesn't exist or lack of permissions.", devicePath));
			continue;
		}

		// Query the device capabilities
		struct v4l2_capability cap;
		if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
			// If querying the device fails, log and skip it
			wxLogError(wxString::Format("Failed to query capabilities for device: %s", devicePath));
			close(fd);
			continue;
		}

		// Check if the device supports the Video Capture interface
		if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
			// If it doesn't support video capture, log and skip it
			wxLogMessage(wxString::Format("Device does not support video capture: %s", devicePath));
			close(fd);
			continue;
		}

		// Device supports video capture, add it to the list and log
		std::string deviceName = reinterpret_cast<char*>(cap.card);
		// Add to devices list
		Devices[counter] = CameraHWDevice(i, std::string(friendlyName));
		wxLogMessage(wxString::Format("Found video capture device: %s, Name: %s", devicePath, deviceName));

		close(fd);
		deviceFound = true;
	}

	if (!deviceFound) {
		wxLogError("No video capture devices found.");
	}

	// Return true if we found at least one device
	return deviceFound;
}
#endif


inline bool ListVideoCaptureDevices(std::unordered_map<int, CameraHWDevice>& Devices)
{
    #ifdef _WIN32
		return ListVideoCaptureDevicesWindows(Devices);
    #elif defined(__APPLE__)
		return ListVideoCaptureDevicesMacOS(Devices);
    #elif defined(__linux__)
		return ListVideoCaptureDevicesLinux(Devices);
    #else
		wxLogError("Unsupported platform.");
		return false;
    #endif
}

