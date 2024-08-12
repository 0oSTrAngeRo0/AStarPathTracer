#pragma once

#define ASTAR_CONCAT_(x,y) x##y
#define ASTAR_CONCAT(x,y) ASTAR_CONCAT_(x,y)
#define ASTAR_UNIQUE_VARIABLE_NAME(prefix) ASTAR_CONCAT(prefix, __COUNTER__)