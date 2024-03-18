#include "utils/Config.h"
#include "utils/AppUtils.h"

#include <wx/stdpaths.h>
#include <wx/filename.h>

bool Config::CheckIfDirExists(const std::string& dirPath)
{
	wxFileName fName;
	fName.AssignDir(GetLocalPath());
	fName.AppendDir(dirPath);
	
	return wxFileName::DirExists(fName.GetPath());
}

bool Config::MakeDir(const std::string& dirPath, bool doRecursive)
{
	wxFileName fName;
	fName.AssignDir(GetLocalPath());
	fName.AppendDir(dirPath);
	
	const int flags = doRecursive ? wxS_DIR_DEFAULT : wxS_DIR_DEFAULT | wxPATH_MKDIR_FULL;
	
	return wxFileName::Mkdir(fName.GetPath(), wxS_DIR_DEFAULT, flags);
}

bool Config::LoadConfig(const std::string& filePath, Config& outConfig)
{
	const mINI::INIStructure ini;
	outConfig = Config(ini);
	
	return outConfig.Load(filePath);
}

bool Config::SaveConfig(const std::string& filePath, Config& config, bool newFile)
{
	return config.Save(filePath, newFile);
}

bool Config::Load(const std::string& filePath)
{
	wxFileName fName;
	fName.AssignDir(GetLocalPath());
	fName.SetName(filePath);
	
	const std::string fullPath = fName.GetFullPath();
	// Create INI file
	const mINI::INIFile file(fullPath);

	// Read INI file
	return wxFileExists(fullPath) && file.read(ini);
}

bool Config::Save(const std::string& filePath, bool newFile)
{
	wxFileName fName;
	fName.AssignDir(GetLocalPath());
	fName.SetName(filePath);
	
	const std::string fullPath = fName.GetFullPath();
	// Create INI file
	const mINI::INIFile file(fullPath);

	// Write INI file
	return newFile ? file.generate(ini, true) : wxFileExists(fullPath) && file.write(ini, true);
}

mINI::INIStructure& Config::GetData()
{
	return ini;
}