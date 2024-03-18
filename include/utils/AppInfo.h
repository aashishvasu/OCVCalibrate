#pragma once

#include "AppUtils.h"

#define SE_MAJ_VER 0
#define SE_MIN_VER 1
#define SE_PAT_VER 0

// Function to generate semantic version string
inline std::string GenerateSemanticVersion()
{
	std::string version = "v" + std::to_string(SE_MAJ_VER) + "." + std::to_string(SE_MIN_VER);

	if (SE_PAT_VER != 0) {
		version += "." + std::to_string(SE_PAT_VER);
	}

	return version;
}