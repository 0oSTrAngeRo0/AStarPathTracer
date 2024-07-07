#pragma once

#include <glm/glm.hpp>
#include <string>

/// <summary>
/// Load mesh data from *.obj file
/// </summary>
/// <param name="path">file path</param>
/// <returns>A tuple: [position(glm::vec3), indices(uin32_t)]</returns>
std::tuple<std::vector<glm::vec3>, std::vector<uint32_t>> LoadFromFile(const std::string& path);