#include "map.h"


inline bool map_is_solid(u16 tile)
{
    return tile <= 64 && tile != 0;
}