#pragma once

#include "Engine/Resources/TextureLoader/TextureResourceUtilities.h"

class StbImageUtilities {
public:
	static TextureData Load(const std::string& path);
};