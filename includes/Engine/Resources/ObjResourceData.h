#include "Engine/Resources/Resources.h"
#include <string>

struct ObjResourceData {
	std::string path; // relative path
	// other import settings
};

template<> constexpr ResourceTypeDisplay Resource<ObjResourceData>::type_display = "Obj";


