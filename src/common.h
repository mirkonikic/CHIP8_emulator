#include "../include/common.h"
#include <iostream>

void bin(WORD w)
{
        for(int i = 8; i>0; i--)
        {
                std::cout<<(((w>>i-1) & 1 == 1)?"1":"0");
        }

        std::cout<<" : "<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)w<<std::dec;

        std::cout<<"\n";
};
