#pragma once

#include <string>
#include <wx/stdpaths.h>

inline std::string GetLocalPath()
{
	return wxStandardPaths::Get().GetLocalDataDir().utf8_string();
}

inline std::string GetResourcePath()
{
	return wxStandardPaths::Get().GetResourcesDir().utf8_string();
}