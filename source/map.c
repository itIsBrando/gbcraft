#include "map.h"


inline bool map_is_solid(uint tile)
{
    return tile <= 64 && tile != 0;
}