#include "data/CalibrationSettings.h"

#include <opencv2/opencv.hpp>

std::string CalibrationSettings::GenerateCalibrationFilename(const int& ID)
{
	return "id"+ std::to_string(ID) + "_calibration.ini";
}

// Function to convert cv::Mat to a comma-separated string
std::string CalibrationSettings::MatToString(const cv::Mat& mat)
{
	std::ostringstream oss;
	for (int i = 0; i < mat.rows; ++i) {
		for (int j = 0; j < mat.cols; ++j) {
			// Append each element to the stringstream, converting to string and separating with commas
			oss << mat.at<double>(i, j);
			if (j < mat.cols - 1) oss << ",";
		}
		// Separate rows by semicolons
		if (i < mat.rows - 1) oss << ";";
	}
	return oss.str();
}

// Function to convert comma separated string to a cv::Mat
cv::Mat CalibrationSettings::StringToMat(const std::string& str, const int& rows, const int& cols)
{
	std::vector<double> data;
	std::istringstream iss(str);
	std::string item;

	while (std::getline(iss, item, ',')) {
		double val = std::stod(item);
		data.push_back(val);
	}

	cv::Mat mat(rows, cols, CV_64F); // Assuming the matrix is of type double
	std::memcpy(mat.data, data.data(), data.size() * sizeof(double));

	return mat;
}

bool CalibrationSettings::Load(const int& id)
{
	return Config::LoadConfig("calibration/"+GenerateCalibrationFilename(id), CalibrationINI);
}

bool CalibrationSettings::Save(const int& id, bool newFile)
{
	return CalibrationINI.Save("calibration/"+GenerateCalibrationFilename(id), newFile);
}

mINI::INIStructure& CalibrationSettings::GetData()
{
	return CalibrationINI.GetData();
}
