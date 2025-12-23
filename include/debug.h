#ifndef DEBUG_H
#define DEBUG_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "cpu.h"
#include "mem.h"
#include <string>
#include <sstream>
#include <iomanip>

#define DEBUG_WIDTH 400
#define DEBUG_HEIGHT 768

class Debugger {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* fontSmall;
    
    cpu_t* cpu;
    memory_t* memory;
    
    bool stepMode;
    bool running;
    int scrollOffset;
    int memoryViewStart;
    
    SDL_Color bgColor = {20, 20, 30, 255};
    SDL_Color textColor = {220, 220, 220, 255};
    SDL_Color highlightColor = {100, 200, 255, 255};
    SDL_Color accentColor = {255, 100, 100, 255};
    SDL_Color successColor = {100, 255, 100, 255};
    SDL_Color panelColor = {30, 30, 45, 255};
    
public:
    Debugger();
    ~Debugger();
    
    bool init(cpu_t* _cpu, memory_t* _mem);
    void render();
    void handleInput(SDL_Event& event);
    void cleanup();
    
    bool isStepMode() { return stepMode; }
    bool isRunning() { return running; }
    void step() { running = true; }
    void pause() { running = false; stepMode = true; }
    void resume() { running = true; stepMode = false; }
    
private:
    void drawText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* f = nullptr);
    void drawRect(int x, int y, int w, int h, SDL_Color color, bool filled = true);
    void drawPanel(int x, int y, int w, int h, const std::string& title);
    
    void renderRegisters();
    void renderStack();
    void renderMemory();
    void renderControls();
    void renderTimers();
    void renderCurrentInstruction();
    
    std::string formatHex(uint16_t value, int width);
    std::string formatBinary(uint8_t value);
};

#endif
