#ifndef _DISPL_H_
#define _DISPL_H_

#include "common.h"
#include <SDL.h>
#include <unistd.h>

#define DISPLAY_MULTIPLIER 10
#define SCREEN_HEIGHT 32*DISPLAY_MULTIPLIER
#define SCREEN_WIDTH 64*DISPLAY_MULTIPLIER

class display_t
{
	private:
		SDL_Window* window = nullptr;
		SDL_Surface* screenSurface = nullptr;
		SDL_Renderer* renderer = nullptr;

	public:
		BYTE screen[32*64];
		//BYTE screen[64][32];
		void init(SDL_Window* wnd, SDL_Renderer* rndr, SDL_Surface* scrn);
		void quit();
		void draw();
		void clear();
		void animation();
};

#endif
