#include "../include/mem.h"
#include <iomanip>
#include <fstream>
#include <iostream>

void memory_t::setCell(int cell, char val)
{
	RAM[cell] = val;
}

//GET CERTAIN CELL - 0x1000 - 0d4096
uint8_t memory_t::getCell(int cell)
{
	return RAM[cell];
}

memory_t::memory_t(void)
{
	for(int i = 0; i<4096; i++)
	{
		RAM[i] = 0;
	}
}

void memory_t::clear(void)
{
	for(int i = 0; i<4096; i++)
	{
		RAM[i] = 0;
	}
}

void memory_t::printmem(int n)
{
	for(int i = 0x200; i<(0x200+n); i++)
	{
		printCell(i);
		//std::cout<<(int)RAM[i]<<"\n";
	}
}

void memory_t::printCell(int cell)
{
	std::cout<<std::setw(3)<<std::setfill('0')<<cell<<". cell: "<<std::setw(3)<<std::setfill('0')<<(int)RAM[cell]<<" : ";

	for(int i = 8; i>0; i--)
	{
		std::cout<<(((RAM[cell]>>i-1) & 1 == 1)?"1":"0");
	}
	
	std::cout<<" : "<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)RAM[cell]<<std::dec;

	std::cout<<"\n";
}

void memory_t::load(std::string f, int n)
{
	std::ifstream rf(f, std::ios::out | std::ios::binary);
	if(!rf) {
		
		return;
	}

	for(int i = 0; i<n; i++)
	{
		rf.read((char *)&RAM[i+0x200], 1);
	}

	printmem(n);
}
