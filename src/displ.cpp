#include "../include/displ.h"

void display_t::init()
{
	for(int x = 0; x<32*64; x++)
	{
		screen[x] = 0;
	}

	//The window we'll be rendering to
	window = NULL;
    
	//The surface contained by the window
	screenSurface = NULL;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
    {
        //Create window
        window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get window renderer
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

            //Get window surface
            //screenSurface = SDL_GetWindowSurface( window );

            //Fill the surface white
            //SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x00, 0xff, 0x00 ) );

            //Update the surface
            //SDL_UpdateWindowSurface( window );

        }
    }

}

void display_t::clear()
{
    printf("clearing display\n");
    for(int x = 0; x<32*64; x++)
	{
        screen[x] = 0;
	}

    draw();
}

void display_t::test()
{
    printf("testing display\n");
    for(int x = 0; x<32*64; x++)
	{
        screen[x] = 0;
        if(x % 2 == 0)
            screen[x] = 1;
	}

    draw();
}

void display_t::draw()
{
    //clear out the canvas
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    printf("drawing on display\n");
    for(int x = 0; x<32*64; x++)
    {
        if(screen[x] == 1){
            //printf("X: %d; Y: %d :) \n", x%64, x/64);
            SDL_RenderDrawPoint(renderer, (x%64)*10, (x/64)*10);
        }
    }

    //print on window
    SDL_RenderPresent(renderer);
}

void display_t::quit()
{
        //Wait two seconds
        SDL_Delay( 2000 );

	    //Destroy window
	    SDL_DestroyWindow( window );

	    //Quit SDL subsystems
        SDL_Quit();
}