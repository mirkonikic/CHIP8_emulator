#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstring>
#include <bitset>
#include <SDL.h>

//class memory_t;
//class cpu_t;
//class display_t;

#include "mem.h"
#include "cpu.h"
#include "displ.h"

typedef uint8_t BYTE;
typedef uint16_t WORD;

#define O_HELP          0x01<<0	//0b00000001 //help
#define O_LIST          0x01<<1	//0b00000010 //list all roms
#define O_RUN           0x01<<2	//0b00000100 //run the game
#define O_EXISTS        0x01<<3	//0b00001000 //check if it exists
#define O_VERBOSE       0x01<<4	//0b00010000 //give more information
#define O_INSPECT       0x01<<5	//0b00100000 //load and print info
#define O_PRINT		0x01<<6	//0b01000000 //load and print hex values
//0b10000000 //speed of executing

bool end = true;

#endif
