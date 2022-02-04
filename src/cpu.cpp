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

	drawFlag = false;
	
	//Load fontset
	for(int i = 0; i < 80; ++i)
		memory->setCell(i, chip8_fontset[i]);

	//Reset timers
	delay_timer = 0;
	sound_timer = 0;

	printf("\tpc: %.2X; \n", pc);

}

int cpu_t::execute()
{
	//fetch opcode
	opcode = memory->getCell(pc)<<8 | memory->getCell(pc+1);
	pc++; pc++;

	printf("pc: %.2X 0x%X ", pc, opcode);

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
					drawFlag = true;
					printf("0x00E0:\n\tDisplay Cleared :D");
					break;
 
				case 0x00EE: // 0x00EE: Returns from subroutine          
					// Execute opcode
					sp--;
                	pc = stack[sp];
                	pc += 2;
                	break;
				default:
					//printf ("Unknown opcode [0x0000]: 0x%X\n", opcode);          
					break;
			}
			break;
		case 0x1000:
			//jumps to address NNN -> jmp ne cuva adresu na stacku
			//samo promeni pc u adresu
			printf("0x1NNN:\n\tBpc: %.2X; ", pc);
			pc = opcode & 0x0FFF;	//2B - pc je 2B :D
			printf("Apc: %.2X; %d \n", pc, pc);
			break;		
		case 0x2000:
			//call subroutine at NNN
			//set stack to pc, increase stackpointer, postavi pc na 2NNN -> NNN
			stack[sp] = pc;
			++sp;
			pc = opcode & 0x0FFF;
			printf("0x2NNN:\n\tsp: %.2X\n", sp);
			break;
		case 0x3000:
			//3XNN -> skip the next instruction if VX == NN
			//vX gde je x jedan od 16 registara
			if(vX[(opcode & 0x0F00) >> 8] == opcode & 0x00FF)
				pc+=2;
			pc += 2;
			printf("0x3XNN:\n\top: %X; X: %X|%d; vX[]: %.2X; pc: %X\n", opcode, (opcode & 0x0F00) >> 8, (opcode & 0x0F00) >> 8, vX[(opcode & 0x0F00) >> 8], pc);
			break;
		case 0x4000:
			//4XNN -> skip the next instruction if the VX != NN
			if(vX[(opcode & 0x0F00) >> 8] != opcode & 0x00FF)
				pc+=2;
			pc += 2;
			printf("0x4XNN:\n\top: %X; X: %X|%d; vX[]: %.2X; pc: %X\n", opcode, (opcode & 0x0F00) >> 8, (opcode & 0x0F00) >> 8, vX[(opcode & 0x0F00) >> 8], pc);
			break;
		case 0x5000:
			//5XY0 -> skip next instruction if vX == vY
			if(vX[(opcode & 0x0F00) >> 8] == vX[(opcode & 0x00F0) >> 4])
				pc+=2;
			pc += 2;
			printf("0x5XY0:\n\top: %X; X: %X|%d; vX[]: %.2X; Y: %X|%d; vY[]: %.2X; pc: %X\n", opcode, (opcode & 0x0F00) >> 8, (opcode & 0x0F00) >> 8, vX[(opcode & 0x0F00) >> 8], (opcode & 0x00F0) >> 4, (opcode & 0x00F0) >> 4, vX[(opcode & 0x00F0) >> 4], pc);
			break;
		case 0x6000:
			//6XNN -> sets vX to NN
			vX[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			pc += 2;
			printf("0x6XNN:\n\top: %X; X: %X|%d; vX[]: %.2X; pc: %X\n", opcode, (opcode & 0x0F00) >> 8, (opcode & 0x0F00) >> 8, vX[(opcode & 0x0F00) >> 8], pc);
			break;
		case 0x7000:
			//7XNN -> adds NN to vX
			vX[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
			pc += 2;
			printf("0x7XNN:\n\top: %X; X: %X|%d; vX[]: %.2X; pc: %X\n", opcode, (opcode & 0x0F00) >> 8, (opcode & 0x0F00) >> 8, vX[(opcode & 0x0F00) >> 8], pc);
			break;
		case 0x8000:
			/*
Opcode	Type	C Pseudo	Explanation
+ 8XY0	Assig	Vx = Vy	Sets VX to the value of VY.
+ 8XY1	BitOp	Vx |= Vy	Sets VX to VX or VY. (Bitwise OR operation);
+ 8XY2	BitOp	Vx &= Vy	Sets VX to VX and VY. (Bitwise AND operation);
+ 8XY3	BitOp	Vx ^= Vy	Sets VX to VX xor VY.
+ 8XY4	Math	Vx += Vy	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not.
+ 8XY5	Math	Vx -= Vy	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
+ 8XY6	BitOp	Vx >>= 1	Stores the least significant bit of VX in VF and then shifts VX to the right by 1.[b]
+ 8XY7	Math	Vx = Vy - Vx	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not.
+ 8XYE	BitOp	Vx <<= 1	Stores the most significant bit of VX in VF and then shifts VX to the left by 1.[b]
			*/
			switch(opcode&0x000F)
			{
				case 0x0:
					vX[(opcode&0x0F00)>>8] = vX[(opcode&0x00F0)>>4];
					pc += 2;
					printf("X: %x | %x -> Y: %X | %x \n", vX[(opcode&0x0F00)>>8], (opcode&0x0F00)>>8, vX[(opcode&0x00F0)>>4], (opcode&0x00F0)>>4);
					break;
				case 0x1:
					vX[(opcode&0x0F00)>>8] |= vX[(opcode&0x00F0)>>4];
					pc += 2;
					vX[0xF] = 0;
					printf("X: %x | %x -> Y: %X | %x \n", vX[(opcode&0x0F00)>>8], (opcode&0x0F00)>>8, vX[(opcode&0x00F0)>>4], (opcode&0x00F0)>>4);
					break;
				case 0x2:
					vX[(opcode&0x0F00)>>8] &= vX[(opcode&0x00F0)>>4];
					vX[0xF] = 0;
					pc += 2;
					printf("X: %x | %x -> Y: %X | %x \n", vX[(opcode&0x0F00)>>8], (opcode&0x0F00)>>8, vX[(opcode&0x00F0)>>4], (opcode&0x00F0)>>4);
					break;
				case 0x3:
					vX[(opcode&0x0F00)>>8] ^= vX[(opcode&0x00F0)>>4];
					vX[0xF] = 0;
					pc += 2;
					printf("X: %x | %x -> Y: %X | %x \n", vX[(opcode&0x0F00)>>8], (opcode&0x0F00)>>8, vX[(opcode&0x00F0)>>4], (opcode&0x00F0)>>4);
					break;
				case 0x4:
					vX[(opcode&0x0F00)>>8] += vX[(opcode&0x00F0)>>4];
					vX[0xF] = (BYTE)(vX[(opcode&0x0F00)>>8]+vX[(opcode&0x00F0)>>4])>0xFF;
					pc += 2;
					printf("X: %x | %x -> Y: %X | %x; carry: %d \n", vX[(opcode&0x0F00)>>8], (opcode&0x0F00)>>8, vX[(opcode&0x00F0)>>4], (opcode&0x00F0)>>4, (BYTE)(vX[(opcode&0x0F00)>>8]+vX[(opcode&0x00F0)>>4])>0xFF);
					break;
				case 0x5:
					vX[(opcode&0x0F00)>>8] -= vX[(opcode&0x00F0)>>4];
					pc += 2;
					vX[0xF] = (BYTE)!(vX[(opcode&0x0F00)>>8]<vX[(opcode&0x00F0)>>4]);
					printf("X: %x | %x -> Y: %X | %x; carry: %d \n", vX[(opcode&0x0F00)>>8], (opcode&0x0F00)>>8, vX[(opcode&0x00F0)>>4], (opcode&0x00F0)>>4, (BYTE)!(vX[(opcode&0x0F00)>>8]<vX[(opcode&0x00F0)>>4]));
					break;
				case 0x6:
					vX[0xF] = vX[(opcode&0x0F00)>>8] & 0x1;
					vX[(opcode&0x0F00)>>8] >>= 1;
					pc += 2;
					printf("X: %x | %x -> Y: %X | %x; vF: %d \n", vX[(opcode&0x0F00)>>8], (opcode&0x0F00)>>8, vX[(opcode&0x00F0)>>4], (opcode&0x00F0)>>4, vX[0xF]);
					break;
				case 0x7:
					vX[(opcode&0x0F00)>>8] = vX[(opcode&0x00F0)>>4] - vX[(opcode&0x0F00)>>8];
					pc += 2;
					vX[0xF] = (BYTE)!(vX[(opcode&0x0F00)>>8]>vX[(opcode&0x00F0)>>4]);
					printf("X: %x | %x -> Y: %X | %x; carry: %d \n", vX[(opcode&0x0F00)>>8], (opcode&0x0F00)>>8, vX[(opcode&0x00F0)>>4], (opcode&0x00F0)>>4, (BYTE)!(vX[(opcode&0x0F00)>>8]<vX[(opcode&0x00F0)>>4]));
					break;
				case 0xE:
					vX[0xF] = vX[(opcode&0x0F00)>>8] & 0b10000000;
					vX[(opcode&0x0F00)>>8] <<= 1;
					pc += 2;
					printf("X: %x | %x -> Y: %X | %x; vF: %d \n", vX[(opcode&0x0F00)>>8], (opcode&0x0F00)>>8, vX[(opcode&0x00F0)>>4], (opcode&0x00F0)>>4, vX[0xF]);
					break;
				default:
					printf("%X unknown opcode\n", opcode);
					pc += 2;
					break;
			}
			break;
		case 0x9000:
			//9XY0 -> skips next instruction if vX != vY
			if(vX[(opcode & 0x0F00) >> 8] != vX[(opcode & 0x00F0) >> 4])
				pc+=2;
			pc += 2;
			printf("0x9XY0:\n\top: %X; X: %X|%d; vX[]: %.2X; Y: %X|%d; vY[]: %.2X; pc: %X\n", opcode, (opcode & 0x0F00) >> 8, (opcode & 0x0F00) >> 8, vX[(opcode & 0x0F00) >> 8], (opcode & 0x00F0) >> 4, (opcode & 0x00F0) >> 4, vX[(opcode & 0x00F0) >> 4], pc);
			break;
		case 0xA000:
			//0xANNN sets I reg to NNN
			I = opcode & 0x0FFF;
			pc += 2;
			printf("0xANNN:\n\top: %X; I: %X|%d; pc: %X\n", opcode, I, I, pc);
			break;
		case 0xB000:
			//BNNN jumps to address v0 + NNN
			pc = vX[0] + (opcode) & 0x0FFF;
			printf("0xBNNN:\n\tpc: %.2X; vX[0]: %X\n", pc, vX[0]);
			break;
		case 0xC000:
			//CXNN sets vx to rand() BITWISE AND NN
		//prepravi u random
			vX[(opcode & 0x0F00) >> 8] = 5/*(rand() % (0xFF + 1))*/ & (opcode & 0x00FF);
            pc += 2;
			printf("0xCXNN:\n\top: %X; X: %X|%d; vX[]: %.2X; pc: %X\n", opcode, (opcode & 0x0F00) >> 8, (opcode & 0x0F00) >> 8, vX[(opcode & 0x0F00) >> 8], pc);
			break;
		case 0xD000:		
printf("0xDXYN - trebalo bi da je gotovo\n\t");
			printf("x: %d; y: %d; N: %d; I: %X\n", vX[((opcode&0x0F00)>>8)], vX[((opcode&0x00F0)>>8)], opcode&0x000F, I);
			//0xDXYN
			//	Draw sprite at position X,Y
			//	N rows width of 8 bits
			//	Sprite is at location on which I points to
			for(int i = 0; i<(opcode&0x000F); i++)
			{
				for(int j = 0; j<8; j++)
				{
					printf("celije: %d; vrednost: %d; memorija: %d\n", (((opcode&0x0F00)>>8) + j) + (((opcode&0x00F0)>>4)+i)*64, memory->getCell(I+(i*8)+j), I+i);
					if ((memory->getCell(I+i) & (0x80 >> j)) != 0)
					{
						if(display->screen[vX[((opcode&0x0F00)>>8)] + j + ( vX[((opcode&0x00F0)>>4)]+i)*64] == 1)
							vX[0xF] = 1;
						display->screen[( vX[((opcode&0x0F00)>>8)] + j) + ( vX[((opcode&0x00F0)>>4)]+i)*64] ^= 1;
					}
				}
				//y = 1 -> jedan red od 64 x je predjen
				//n / 32 = red u kom se trenutno nalazi
				//n % 64 = celiju u ovom redu
				//da bi dobili nazad element
				//x + ((y+i) * 64)
				//ali na tome crtamo po 8 pixela width
			}
			drawFlag = true;
			pc += 2;
			break;
		case 0xE000:
//zavrsi input
			//EX9E - if the key stored in vX is pressed skips next instr
			//EXA1 - if the key stored in vX is not pressed skips next instr
			switch(opcode & 0x00FF)
			{
				case 0x9E:
				/*
                    //Skips the next instruction if the key stored in VX is pressed.
                    reg = get_nibble(opcode, 8, 0x0F00);
                    pc += 2;
                    if (keypad[V[reg]] != 0)
                    {
                        pc += 2;
                    }
				*/
					printf("0xEX9E - not finished\n");
					break;
				case 0xA1:
                /*
				    //Skips the next instruction if the key stored in VX isn't pressed.
                    reg = get_nibble(opcode, 8, 0x0F00);
                    pc += 2;
                    if (keypad[V[reg]] == 0)
                    {
                        pc += 2;
                    }
				*/
					printf("0xEXA1 - not finished\n");
					break;
			}
			break;
		case 0xF000:
			//std::cout<<"FXXX";
			/*
+ FX07	Timer	Vx = get_delay()	Sets VX to the value of the delay timer.
+ FX0A	KeyOp	Vx = get_key()	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event);
+ FX15	Timer	delay_timer(Vx)	Sets the delay timer to VX.
+ FX18	Sound	sound_timer(Vx)	Sets the sound timer to VX.
+ FX1E	MEM	I += Vx	Adds VX to I. VF is not affected.
+ FX29	MEM	I = sprite_addr[Vx]	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
- FX33	BCD	set_BCD(Vx)
*(I+0) = BCD(3);
*(I+1) = BCD(2);
*(I+2) = BCD(1);
Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.);
- FX55	MEM	reg_dump(Vx, &I)	Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[d]
- FX65	MEM	reg_load(Vx, &I)	Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[d]
			*/
			switch(opcode & 0x00FF)
			{
				case 0x07:
					vX[(opcode & 0x0F00) >> 8] = delay_timer;
					pc += 2;
					printf("X: %x; d_time: %x \n", vX[(opcode & 0x0F00) >> 8], delay_timer);
					break;
//zavrsi
				case 0x0A:
					//vX[(opcode & 0x0F00) >> 8] = kbd->get_key();
					pc += 2;
					printf("KEY PRESS EXPECTED \n");
					break;
				case 0x15:
					delay_timer = vX[(opcode & 0x0F00) >> 8];
					pc += 2;
					break;
				case 0x18:
					sound_timer = vX[(opcode & 0x0F00) >> 8];
					pc += 2;
					break;
				case 0x1E:
					I += vX[(opcode & 0x0F00) >> 8];
					pc += 2;
					break;
				case 0x29:
					I = vX[(opcode & 0x0F00) >> 8] * 0x5;
					pc += 2;
					break;
				case 0x33:
                    memory->setCell(I, vX[(opcode & 0x0F00) >> 8] / 100);
                    memory->setCell(I+1, ((vX[(opcode & 0x0F00) >> 8] / 10) % 10));
                    memory->setCell(I+2, ((vX[(opcode & 0x0F00) >> 8] % 100) % 10));
                    pc += 2;
					break;
				case 0x55:
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
                    {
                        memory->setCell(I + i, vX[i]);
                    }
                    I = I + ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
					break;
				case 0x66:	
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
                    {
                        vX[i] = memory->getCell(I + i);
                    }
                    I = I + ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
					break;
			}
			break;
		default:
			printf ("Unknown opcode: 0x%X\n", opcode);
			return 1;
			break;
	}		

	
	//update timers
	if(delay_timer > 0)
		--delay_timer;
 
	if(sound_timer > 0)
	{
		if(sound_timer == 1)
			printf("BEEP!\n");
		--sound_timer;
	} 
	

	usleep(100000);

	return 0;	
}


