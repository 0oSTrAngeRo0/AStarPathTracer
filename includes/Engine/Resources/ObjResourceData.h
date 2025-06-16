#include <string>
#include "Engine/Resources/Resources.h"

struct ObjResourceData {
public:
    static constexpr ResourceTypeDisplay type_display = "Obj";
	std::string path; // relative path
	// other import settings
};