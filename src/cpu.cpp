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
	display->clear();
	//Clear stack & registers V0-VF
	for(int i = 0; i<16; i++){vX[i] = 0; stack[i]=0;}
	//Clear memory
	memory->clear();
	
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
			switch(opcode & 0x00FF)
			{
				case 0x00E0: // 0x00E0: Clears the screen        
					// Execute opcode
					display->clear();
					break;
 
				case 0x00EE: // 0x00EE: Returns from subroutine          
					// Execute opcode
					break;
				default:
					//calls machine code routine at address NNN
					//printf ("Unknown opcode [0x0000]: 0x%X\n", opcode);          
					break;
			}
			break;
		case 0x1000:
			//jumps to address NNN -> jmp ne cuva adresu na stacku
			//samo promeni pc u adresu
			break;
		case 0x2000:
			//call subroutine at NNN
			//set stack to pc, increase stackpointer, postavi pc na 2NNN -> NNN
			stack[sp] = pc;
			++sp;
			pc = opcode & 0x0FFF;
			break;
		case 0x3000:
			//3XNN -> skip the next instruction if VX == NN
			//vX gde je x jedan od 16 registara
			break;
		case 0x4000:
			//4XNN -> skip the next instruction if the VX != NN
			break;
		case 0x5000:
			//5XY0 -> skip next instruction if vX == vY
			break;
		case 0x6000:
			//6XNN -> sets vX to NN
			break;
		case 0x7000:
			//7XNN -> adds NN to vX
			break;
		case 0x8000:
			/*
Opcode	Type	C Pseudo	Explanation
8XY0	Assig	Vx = Vy	Sets VX to the value of VY.
8XY1	BitOp	Vx |= Vy	Sets VX to VX or VY. (Bitwise OR operation);
8XY2	BitOp	Vx &= Vy	Sets VX to VX and VY. (Bitwise AND operation);
8XY3[a]	BitOp	Vx ^= Vy	Sets VX to VX xor VY.
8XY4	Math	Vx += Vy	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not.
8XY5	Math	Vx -= Vy	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
8XY6[a]	BitOp	Vx >>= 1	Stores the least significant bit of VX in VF and then shifts VX to the right by 1.[b]
8XY7[a]	Math	Vx = Vy - Vx	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not.
8XYE[a]	BitOp	Vx <<= 1	Stores the most significant bit of VX in VF and then shifts VX to the left by 1.[b]
			*/
			break;
		case 0x9000:
			//9XY0 -> skips next instruction if vX != vY
			break;
		case 0xA000:
			//0xANNN sets I reg to NNN
			I = opcode & 0x0FFF;
			pc += 2;
			break;
		case 0xB000:
			//BNNN jumps to address v0 + NNN
			pc = vX[0] + (opcode) & 0x0FFF;
			break;
		case 0xC000:
			//CXNN sets vx to rand() BITWISE AND NN
			break;
		case 0xD000:
			//0xDXYN
			//	Draw sprite at position X,Y
			//	N rows width of 8 bits
			//	Sprite is at location on which I points to
			break;
		case 0xE000:
			//EX9E - if the key stored in vX is pressed skips next instr
			//EXA1 - if the key stored in vX is not pressed skips next instr
			break;
		case 0xF000:
			//std::cout<<"FXXX";
			/*
FX07	Timer	Vx = get_delay()	Sets VX to the value of the delay timer.
FX0A	KeyOp	Vx = get_key()	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event);
FX15	Timer	delay_timer(Vx)	Sets the delay timer to VX.
FX18	Sound	sound_timer(Vx)	Sets the sound timer to VX.
FX1E	MEM	I += Vx	Adds VX to I. VF is not affected.[c]
FX29	MEM	I = sprite_addr[Vx]	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
FX33	BCD	
set_BCD(Vx)
*(I+0) = BCD(3);
*(I+1) = BCD(2);
*(I+2) = BCD(1);
Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.);
FX55	MEM	reg_dump(Vx, &I)	Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[d]
FX65	MEM	reg_load(Vx, &I)	Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[d]
			*/
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

