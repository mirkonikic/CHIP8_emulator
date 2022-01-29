#ifndef _DISPL_H_
#define _DISPL_H_

#include "common.h"

class display_t
{
	private:

	public:
		BYTE screen[32*64];
		void init();
};

#endif
