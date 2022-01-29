#include "../include/main.h"

std::filesystem::path f;	//rom path
//Preko maske odredjujem sta treba da uradim, dal se trazi run, help ili list
//I prenosim informacije, dal postoji fajl
//Za to mi je trebalo 8 bitova, pa koristim unsigned char

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

	cpu_t cpu;
	memory_t memory;
	display_t display;
		
	//povezati cpu i memoriju
	memory.init();
	display.init();
	cpu.init(&memory, &display);
	//gpu.init();
	//io.init();

	//cpu.memtest(12);

	memory.load(f, std::filesystem::file_size(f));
	
	while(!end)
	{
		if(cpu.execute() == 1)
			end = true;

		//drawgpu();
		//readio();
	}

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
	if(bitmask & O_RUN) std::cout<<"Executing.."<<std::endl;

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
