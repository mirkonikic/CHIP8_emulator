#include "../include/cpu.h"

//NAPRAVI da ispise instrukcije iz memorije ili hexa vrednost :)
void cpu_t::memtest(int n)
{
	std::cout<<"memtest:\n";
	for(int i = 0; i<12; i++)
	{
		memory->setCell(i, i);
	}

	memory->printmem(12);
}
