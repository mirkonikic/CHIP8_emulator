#include <iostream>
//#include <filesystem>
#include <fstream>
#include <experimental/filesystem>
#include <string>

void rom_print(std::string s);
bool rom_exists(std::string s);

int main(int argc, char **argv)
{
	rom_print(argv[1]);
	return 0;
}

bool rom_exists(std::string s)
{
	std::experimental::filesystem::v1::path f = "roms/"+s;
	
	std::cout<<f<<" and "<<std::experimental::filesystem::v1::exists(f)<<" and is of size: "<<std::experimental::filesystem::v1::file_size(f)<<std::endl;
	return std::experimental::filesystem::v1::exists(f);	
}

void rom_print(std::string s)
{
	rom_exists(s);
}
