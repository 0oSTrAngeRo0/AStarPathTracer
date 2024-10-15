#pragma once

#include <string>

struct Name
{
	std::string name; // may be use fixed size string?
	Name() = default;
	Name(const std::string& name) : name(name) {}
	operator const std::string() const { return name; }
	operator std::string() const { return name; }
};