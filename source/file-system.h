#ifndef PATHTRACER_FILE_SYSTEM_H
#define PATHTRACER_FILE_SYSTEM_H

#include <vector>
#include <fstream>

namespace asset {
    std::vector<char> LoadBinaryFile(const char *filename);
}

#endif //PATHTRACER_FILE_SYSTEM_H
