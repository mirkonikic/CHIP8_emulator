SRC = $(wildcard src/*.cpp)
INC = $(wildcard include/*.h)
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

all: 
	g++ $(SRC) $(INC) -o bin/chip8 -lstdc++fs -std=c++17 `sdl2-config --cflags --libs`
	sudo chmod +x bin/chip8

clean:
	rm -rf bin/chip8 
