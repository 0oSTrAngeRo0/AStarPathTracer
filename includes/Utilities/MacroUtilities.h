#pragma once

#include <cassert>
#include <cstdio>

#define ASTAR_CONCAT_(x,y) x##y
#define ASTAR_CONCAT(x,y) ASTAR_CONCAT_(x,y)
#define ASTAR_UNIQUE_VARIABLE_NAME(prefix) ASTAR_CONCAT(prefix, __COUNTER__)
#define ASTAR_ASSERT(expression) assert(expression)
#define ASTAR_DO_NOTHING(...)
#define ASTAR_PRINT(fmt, ...) std::printf(fmt, __VA_ARGS__)