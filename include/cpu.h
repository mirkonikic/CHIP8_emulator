#ifndef _CPU_H_
#define _CPU_H_

#include "mem.h"
#include "displ.h"
#include "common.h"
#include <unistd.h>

class cpu_t
{
	private:
		memory_t *memory = nullptr;
		display_t *display = nullptr;
		//keypad_t *keypad = nullptr;
		WORD pc, I, opcode, sp, stack[16];
		BYTE vX[16], delay_timer, sound_timer;
//vidi ili ovako da implementiras kbd ili kao odvojenu klasu
		//delay timer & sound timer
		//64x32 pixels display
		//gpu drawn by sprites, 8w 1-15h

	public:
		bool drawFlag = false;
		bool key_pressed = false;
		int keypad[16];
		void init(memory_t *mem, display_t *displ);
		int execute();
		void memtest(int n);
		unsigned char chip8_fontset[80] =
		{
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
			0x20, 0x60, 0x20, 0x20, 0x70, // 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
			0x90, 0x90, 0xF0, 0x10, 0x10, // 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
			0xF0, 0x10, 0x20, 0x40, 0x40, // 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
			0xF0, 0x90, 0xF0, 0x90, 0x90, // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
			0xF0, 0x80, 0x80, 0x80, 0xF0, // C
			0xE0, 0x90, 0x90, 0x90, 0xE0, // D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
			0xF0, 0x80, 0xF0, 0x80, 0x80  // F
		};
};

#endif
