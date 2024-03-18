#pragma once

#include <string>
#include <unordered_map>
 
// Structure to hold device names and IDs
struct CameraHWDevice
{
	int ID = 0;
	std::string Name = "New Camera";

	CameraHWDevice() = default;
	CameraHWDevice(int id, std::string name) : ID(id), Name(std::move(name)) {}
	CameraHWDevice(const CameraHWDevice& other) : ID(other.ID), Name(other.Name) {}
};

struct CameraInfo
{
	CameraHWDevice device;

	CameraInfo() = default;
	CameraInfo(const CameraHWDevice& device) : device(device) {}
	
	static void SetDeviceList(std::unordered_map<int, CameraHWDevice> Devices);
	static std::unordered_map<int, CameraHWDevice> GetDeviceList();

private:
	static std::unordered_map<int, CameraHWDevice> DeviceList;
};
