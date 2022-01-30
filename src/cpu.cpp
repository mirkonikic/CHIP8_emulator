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

void cpu_t::init(memory_t *mem, display_t *displ)
{
	//initialize memory
	memory = mem;
	display = displ;

	//initialize registers
	pc = 0x200;	//Program counter starts at 0x200
	opcode = 0;	//Reset current opcode
	sp = 0;		//Reset index register
	I = 0;		//Reset stack pointer

	//Clear display
	//Clear stack
	//Clear registers V0-VF
	//Clear memory
	
	//Load fontset
	for(int i = 0; i < 80; ++i)
		memory->setCell(i, chip8_fontset[i]);

	//Reset timers
	delay_timer = 0;
	sound_timer = 0;
}

int cpu_t::execute()
{
	//fetch opcode
	opcode = memory->getCell(pc)<<8 | memory->getCell(pc+1);
	pc++; pc++;

	//decode & execute opcode
	switch(opcode & 0xF000)
	{
		//uglavnom su prva 4b dovoljna za prepoznavanje
		//popuni svih 35 opkoda
		
		case 0x0000:
			switch(opcode & 0x000F)
			{
				case 0x0000: // 0x00E0: Clears the screen        
					// Execute opcode
					break;
 
				case 0x000E: // 0x00EE: Returns from subroutine          
					// Execute opcode
					break;
 
				default:
					//printf ("Unknown opcode [0x0000]: 0x%X\n", opcode);          
					break;
			}
			break;
		case 0x1000:
			break;
		case 0x2000:
			stack[sp] = pc;
			++sp;
			pc = opcode & 0x0FFF;
			break;
		case 0x3000:
		case 0x4000:
		case 0x5000:
		case 0x6000:
		case 0x7000:
		case 0x8000:
		case 0x9000:
		case 0xA000:
			//0xANNN sets I reg to NNN
			I = opcode & 0x0FFF;
			pc += 2;
			break;
		case 0xB000:
		case 0xC000:
		case 0xD000:
			//0xDXYN
			//	Draw sprite at position X,Y
			//	N rows width of 8 bits
			//	Sprite is at location on which I points to
			break;
		case 0xE000:
		case 0xF000:
			//std::cout<<"FXXX";
			break;
		default:
			//printf ("Unknown opcode: 0x%X\n", opcode);
			return 1;
			break;
	}		

	/*
	//update timers
	if(delay_timer > 0)
		--delay_timer;
 
	if(sound_timer > 0)
	{
		if(sound_timer == 1)
			printf("BEEP!\n");
		--sound_timer;
	} 
	*/

	return 0;	
}


