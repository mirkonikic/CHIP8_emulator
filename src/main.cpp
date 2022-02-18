#include "../include/main.h"

std::filesystem::path f;	//rom path
//Preko maske odredjujem sta treba da uradim, dal se trazi run, help ili list
//I prenosim informacije, dal postoji fajl
//Za to mi je trebalo 8 bitova, pa koristim unsigned char

//keymap[16]	- za keypad
//space			- za pause
//esc			- za exit
//f5			- za reset emulatora
//i				- step trough/continue instructions
//n				- next instruction
//u $REG value	- set register/cell to value
//m				- sound on/off
uint8_t keymap[16] = {
        SDLK_x,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_q,
        SDLK_w,
        SDLK_e,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_z,
        SDLK_c,
        SDLK_4,
        SDLK_r,
        SDLK_f,
        SDLK_v,
};

void binary_print(uint8_t);
uint8_t parse_args(int argc, char** argv);
void execute_args(uint8_t bitmask);

int help_print();

void rom_inspect();
void rom_print(std::string s);
void rom_list();
bool rom_check(std::string s);

int main(int argc, char **argv)
{
//RAZMISLI DAL CES OVAKO, PARSE ARGS MOZE DA VRATI BIT_FIELD NA OSNOVU KOG EXECUTE IZVRSI SVE
//ILI PARSE_ARGS POPUNI GLOBALNU PROMENLJIVU FLAG_MASK
	execute_args(parse_args(argc, argv));	//-h::help, -r::run, -l::list

	if(end) return 0;
	std::cout<<"Executing.."<<std::endl;

	cpu_t cpu;
	memory_t memory;
	display_t display;
	
	SDL_Window* wnd = nullptr;
	SDL_Surface* scrn = nullptr;
	SDL_Renderer* rndr = nullptr;
	
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
    {
        //Create window
        //window = wnd;
        wnd = SDL_CreateWindow( "Mirko's CHIP8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( wnd == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get window renderer
            rndr = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_SOFTWARE);

            //Get window surface
            //screenSurface = SDL_GetWindowSurface( window );

            //Fill the surface white
            //SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x00, 0xff, 0x00 ) );

            //Update the surface
            //SDL_UpdateWindowSurface( window );

        }
    }

	//povezati cpu i memoriju
	memory.init();
	display.init(wnd, rndr, scrn);
	cpu.init(&memory, &display);
	//keyboard.init();

	//cpu.memtest(12);
	display.animation();
	sleep(3);
	display.clear();
	sleep(1);

	memory.load(f, std::filesystem::file_size(f));

	while(!end)
	{
		if(cpu.execute() == 1)
			end = true;

		//usleep(4000);

		//ako je podesen draw flag display.refresh();
		if(cpu.drawFlag == true){
			display.draw();
			cpu.drawFlag = false;
		}

		//readio();
		SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit(0);
            }

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    exit(0);
                }

                for (int i = 0; i < 16; ++i)
                {
                    if (event.key.keysym.sym == keymap[i])
                    {
                        cpu.keypad[i] = 1;
                    }
                }
            }

            if (event.type == SDL_KEYUP)
            {
                for (int i = 0; i < 16; ++i)
                {
                    if (event.key.keysym.sym == keymap[i])
                    {
                        cpu.keypad[i] = 0;
                    }
                }
            }
		}
	}

	display.quit();

	return 0;
}

void rom_inspect()
{
	std::cout<<f<<" "<<std::filesystem::file_size(f)<<"B "<<std::endl;
}

void rom_list()
{
	for(auto file : std::filesystem::directory_iterator("roms/")){
		std::string t = file.path();
		std::string token = t.substr(t.find("/")+1, t.find(".ch8"));
		std::cout<<token<<"\n"; 
	}
	std::cout<<std::endl;
}

bool rom_check(std::string s){f = "roms/"+s/*+".ch8"*/; return std::filesystem::exists(f);}

void rom_print(std::string s)
{
	std::cout<<(rom_check(f)==1?"exists :D":"doesnt exist :/")<<std::endl;

	//load rom rb
	//print hexa svaku instrukciju
}

void execute_args(uint8_t bitmask)
{
	//Precendence of options: help - list - insp - verb - run
	if(bitmask & O_HELP && help_print()) return;	//if print_help() => int/bool
	if(bitmask & O_LIST) return rom_list();		//lists rom files
	if(bitmask & O_INSPECT) rom_inspect();
	if(bitmask & O_VERBOSE) std::cout<<"Verbose set"<<std::endl;
	if(bitmask & O_RUN && end) end=false;

	binary_print(bitmask);
}

uint8_t parse_args(int argc, char** argv)
{
	if(argc < 2){help_print(); return 0;}
	BYTE flag_mask = 0;

	for(int i = 1; i<argc; i++)
	{
		//Da li zapocinju sa -
		//Da li ih ima vise u jednoj npr -LaR ili -l -a...
		//Ako da onda slovo po slovo predji :D
		if(argv[i][0] == '-')
		{
			char *arg = argv[i];	//copy whole string to arg
			arg[0] = '\n';		//set first char to 00
			memmove(arg, arg+1, strlen(arg));	//move whole string to after that first char
		
			for(int j = 0; j<strlen(arg); j++)
			{
				
				switch(arg[j])
				{
					case 'h':
						flag_mask |= O_HELP;		
						break;
					case 'l':
						flag_mask |= O_LIST;
						break;
					case 'r':
						if(i+1<argc && argv[i+1][0] != '-' && rom_check(argv[i+1])){
							flag_mask |= O_EXISTS | O_RUN;
							i++;}
						else if(i+1<argc && argv[i+1][0] == '-')
							flag_mask |= O_HELP;
						else
							std::cout<<"rROM: "<<argv[i+1]<<" doesnt exist"<<std::endl;
						break;
					case 'v':
//IMPLEMENT VERBOSE OPTION
						break;
					case 'i':
						if(i+1<argc && argv[i+1][0] != '-' && rom_check(argv[i+1]))
							flag_mask |= O_INSPECT;
						else if(i+1<argc && argv[i+1][0] == '-')
							flag_mask |= O_HELP;
						else
							std::cout<<"iROM: "<<argv[i+1]<<" doesnt exist"<<std::endl;
						break;
					case 'p':
						if(i+1<argc && argv[i+1][0] != '-' && rom_check(argv[i+1]))
                                                        flag_mask |= O_PRINT;
                                                else if(i+1<argc && argv[i+1][0] == '-')
                                                        flag_mask |= O_HELP;
                                                else
                                                        std::cout<<"iROM: "<<argv[i+1]<<" doesnt exist"<<std::endl;

						break;
					default:
						std::cout<<"Error "<<arg[j]<<" nije opcija"<<std::endl;
						break;
				}
			}
		}
		else
		{
			std::cout<<"Error: "<<argv[i]<<" ne pocinje sa '-'"<<std::endl;
		}	
	}
	binary_print(flag_mask);
	return flag_mask;
}

int help_print(){std::cout<<"Usage: ./chip8 [OPTION]\n\t-h\t : help\n\t-l\t : list\n\t-r <FILE_NAME>\t : run file_name program\n\t-i <FILE_NAME>\t : inspect file_name program\n\tALSO you are able to combine commands like: \t -ir <FILE_NAME>..."<<std::endl; return 1;}
void binary_print(uint8_t bitmask){std::bitset<8> y(bitmask); std::cout<<y<<std::endl;}
