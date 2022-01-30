#ifndef _DISPL_H_
#define _DISPL_H_

#include "common.h"
#include <SDL.h>

#define SCREEN_HEIGHT 320
#define SCREEN_WIDTH 640

class display_t
{
	private:

	public:
		BYTE screen[32*64];
		void init();
};

#endif
