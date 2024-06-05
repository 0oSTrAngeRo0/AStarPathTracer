#ifndef PATHTRACER_FILE_SYSTEM_H
#define PATHTRACER_FILE_SYSTEM_H

#include <vector>
#include <fstream>

namespace asset {
    template <typename T>
    std::vector<T> LoadBinaryFile(const char* filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to load file!");
		}
		size_t file_size = file.tellg();
		if (file_size % sizeof(T) != 0) {
			file.close();
			throw std::runtime_error("Failed to cast to T");
		}
		std::vector<char> buffer(file_size);
		file.seekg(0);
		file.read(buffer.data(), file_size);
		file.close();
		return reinterpret_cast<std::vector<T>&>(buffer);
    }
}

#endif //PATHTRACER_FILE_SYSTEM_H
