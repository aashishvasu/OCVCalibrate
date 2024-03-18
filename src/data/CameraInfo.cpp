#include "data/CameraInfo.h"

#include <utility>

std::unordered_map<int, CameraHWDevice> CameraInfo::DeviceList;

void CameraInfo::SetDeviceList(std::unordered_map<int, CameraHWDevice> Devices)
{
	DeviceList = std::move(Devices);
}

std::unordered_map<int, CameraHWDevice> CameraInfo::GetDeviceList()
{
	return DeviceList;
}
