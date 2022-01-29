#include "../include/displ.h"

void display_t::init()
{
	for(int x = 0; x<32*64; x++)
	{
		screen[x] = 0;
	}
}
