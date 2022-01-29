#ifndef _MEM_H_
#define _MEM_H_

#include <cstdint>
#include <iostream>

class memory_t
{
	private:
		uint8_t RAM[4096];
	public:
		void init() {std::cout<<"initialization of memory complete!\n";};
		void setCell(int cell, char val); //decimal and char
		uint8_t getCell(int cell);	//decimal 0 - 4096
		void printmem(int n);
		void printCell(int cell);
		memory_t();
};

#endif
