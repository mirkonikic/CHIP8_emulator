#include "../include/main.h"
#include "../include/debug.h"

std::filesystem::path f;

SDL_Keycode keymap[16] = {
    SDLK_x,    // 0
    SDLK_1,    // 1
    SDLK_2,    // 2
    SDLK_3,    // 3
    SDLK_q,    // 4
    SDLK_w,    // 5
    SDLK_e,    // 6
    SDLK_a,    // 7
    SDLK_s,    // 8
    SDLK_d,    // 9
    SDLK_z,    // A
    SDLK_c,    // B
    SDLK_4,    // C
    SDLK_r,    // D
    SDLK_f,    // E
    SDLK_v,    // F
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
    execute_args(parse_args(argc, argv));
    
    if(end) return 0;
    std::cout<<"Executing.."<<std::endl;
    
    cpu_t cpu;
    memory_t memory;
    display_t display;
    Debugger debugger;
    
    SDL_Window* wnd = nullptr;
    SDL_Surface* scrn = nullptr;
    SDL_Renderer* rndr = nullptr;
    
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    
    wnd = SDL_CreateWindow("Mirko's CHIP8 Emulator", 
                          SDL_WINDOWPOS_UNDEFINED, 
                          SDL_WINDOWPOS_UNDEFINED, 
                          SCREEN_WIDTH, SCREEN_HEIGHT, 
                          SDL_WINDOW_SHOWN);
    
    if(wnd == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    
    rndr = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED);
    
    memory.init();
    display.init(wnd, rndr, scrn);
    cpu.init(&memory, &display);
    
    if (!debugger.init(&cpu, &memory))
    {
        printf("Failed to initialize debugger!\n");
    }
    
    memory.load(f, std::filesystem::file_size(f));
    
    const int INSTRUCTIONS_PER_FRAME = 10;
    bool debuggerEnabled = true;
    
    while(!end)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                end = true;
                break;
            }
            
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    end = true;
                    break;
                }
                
                if (event.key.keysym.sym == SDLK_F10)
                {
                    debuggerEnabled = !debuggerEnabled;
                }
                
                if (debuggerEnabled)
                {
                    debugger.handleInput(event);
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
        
        if (debuggerEnabled)
        {
            if (debugger.isRunning())
            {
                int instrCount = debugger.isStepMode() ? 1 : INSTRUCTIONS_PER_FRAME;
                
                for(int i = 0; i < instrCount; i++)
                {
                    if(cpu.execute() == 1)
                    {
                        end = true;
                        break;
                    }
                }
                
                if (debugger.isStepMode())
                {
                    debugger.pause();
                }
            }
        }
        else
        {
            for(int i = 0; i < INSTRUCTIONS_PER_FRAME; i++)
            {
                if(cpu.execute() == 1)
                {
                    end = true;
                    break;
                }
            }
        }
        
        if(cpu.drawFlag)
        {
            display.draw();
            cpu.drawFlag = false;
        }
        
        if (debuggerEnabled)
        {
            debugger.render();
        }
        
        SDL_Delay(16);  // ~60 FPS
    }
    
    debugger.cleanup();
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

bool rom_check(std::string s){f = "roms/"+s; return std::filesystem::exists(f);}

void rom_print(std::string s)
{
    std::cout<<(rom_check(f)==1?"exists :D":"doesnt exist :/")<<std::endl;
}

void execute_args(uint8_t bitmask)
{
    if(bitmask & O_HELP && help_print()) return;
    if(bitmask & O_LIST) return rom_list();
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
        if(argv[i][0] == '-')
        {
            char *arg = argv[i];
            arg[0] = '\n';
            memmove(arg, arg+1, strlen(arg));
            
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

int help_print()
{
    std::cout<<"Usage: ./chip8 [OPTION]\n\t-h\t : help\n\t-l\t : list\n\t-r <FILE_NAME>\t : run file_name program\n\t-i <FILE_NAME>\t : inspect file_name program\n\tALSO you are able to combine commands like: \t -ir <FILE_NAME>..."<<std::endl;
    return 1;
}

void binary_print(uint8_t bitmask)
{
    std::bitset<8> y(bitmask);
    std::cout<<y<<std::endl;
}
