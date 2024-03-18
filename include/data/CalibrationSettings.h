#pragma once

#include "utils/Config.h"

#include <string>

namespace cv
{
	class Mat;
}

class CalibrationSettings
{
public:
	static std::string GenerateCalibrationFilename(const int& ID);
	static std::string MatToString(const cv::Mat& mat);
	static cv::Mat StringToMat(const std::string& str, const int& rows, const int& cols);
	
	bool Load(const int& id);
	bool Save(const int& id, bool newFile = false);

	mINI::INIStructure& GetData();
	
	Config CalibrationINI;
};
