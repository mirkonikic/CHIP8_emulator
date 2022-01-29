#ifndef _CPU_H_
#define _CPU_H_

#include "mem.h"
#include "displ.h"
#include "common.h"

class cpu_t
{
	private:
		memory_t *memory = nullptr;
		display_t *display = nullptr;
		WORD pc, I, opcode, sp, stack[16];
		BYTE vX[16], delay_timer, sound_timer;
//vidi ili ovako da implementiras kbd ili kao odvojenu klasu
		//delay timer & sound timer
		//64x32 pixels display
		//gpu drawn by sprites, 8w 1-15h

	public:
		void init(memory_t *mem, display_t *displ);
		int execute();
		void memtest(int n);
};

#endif
