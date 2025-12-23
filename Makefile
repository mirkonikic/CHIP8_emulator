all:
	g++ src/cpu.cpp src/displ.cpp src/main.cpp src/mem.cpp src/debug.cpp \
	include/common.h include/cpu.h include/displ.h include/main.h include/mem.h include/debug.h \
	-o bin/chip8 -lstdc++fs -std=c++17 `sdl2-config --cflags --libs` -lSDL2_ttf
