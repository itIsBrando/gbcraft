#include "defines.h"


void memcpy16(vu16 *dst, const u16 *src, uint hwsize)
{
	for(uint i = 0; i < hwsize; i++)
	{
		*dst = *src;
		dst++, src++;
	}
}