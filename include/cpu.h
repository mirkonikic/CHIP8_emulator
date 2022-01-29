#ifndef _CPU_H_
#define _CPU_H_

#include "mem.h"

class cpu_t
{
	private:
		memory_t *memory = nullptr;
	public:
		void init(memory_t *mem) {memory = mem; };
		void memtest(int n);
};

#endif
