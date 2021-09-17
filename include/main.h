#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstring>
#include <bitset>

#define O_HELP          0x01<<0	//0b00000001
#define O_LIST          0x01<<1	//0b00000010
#define O_RUN           0x01<<2	//0b00000100
#define O_EXISTS        0x01<<3	//0b00001000
#define O_VERBOSE       0x01<<4	//0b00010000
#define O_INSPECT       0x01<<5	//0b00100000
//not available yet 0b01000000
//not available yet 0b10000000
