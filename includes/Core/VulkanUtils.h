#pragma once

import vulkan_hpp;
#include "sstream"

#define DO_WHILE_WRAPPER(inner) do { \
    inner \
} while(0)

#define VK_CHECK(function) DO_WHILE_WRAPPER( \
    vk::Result result = function; \
    if(result == vk::Result::eSuccess) break;          \
	std::ostringstream stream;                  \
	stream << "Vulkan error on [" << #function << "] with error code [" << vk::to_string(result) << "]\n"; \
	stream << __FILE__ << " (at " << __FUNCTION__ << ":" << __LINE__ << ")\n\n"; \
    throw std::runtime_error(stream.str().data()); \
)

#define THROW(error) DO_WHILE_WRAPPER( \
	std::ostringstream stream;  \
	stream << error << '\n' << __FILE__ << " (at " << __FUNCTION__ << ":" << __LINE__ << ")\n\n"; \
    throw std::runtime_error(stream.str().data()); \
)
