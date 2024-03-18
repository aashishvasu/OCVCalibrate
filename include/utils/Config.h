#pragma once
#include "mini/ini.h"

class Config
{
public:
	Config() = default;
	Config(const mINI::INIStructure& iniStructure) : ini(iniStructure){}

	static bool CheckIfDirExists(const std::string& dirPath);
	static bool MakeDir(const std::string& dirPath, bool doRecursive = false);
	
	static bool LoadConfig(const std::string& filePath, Config& outConfig);
	static bool SaveConfig(const std::string& filePath, Config& config, bool newFile = false);

	bool Load(const std::string& filePath);
	bool Save(const std::string& filePath, bool newFile = false);
	
	mINI::INIStructure& GetData();

private:	
	mINI::INIStructure ini;
};
