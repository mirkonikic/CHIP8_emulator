#include "../include/debug.h"
#include <algorithm>

Debugger::Debugger() : window(nullptr), renderer(nullptr), font(nullptr), 
                       fontSmall(nullptr), cpu(nullptr), memory(nullptr),
                       stepMode(false), running(true), scrollOffset(0), 
                       memoryViewStart(0x200) {}

Debugger::~Debugger() {
    cleanup();
}

bool Debugger::init(cpu_t* _cpu, memory_t* _mem) {
    cpu = _cpu;
    memory = _mem;
    
    if (TTF_Init() == -1) {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        return false;
    }
    
    window = SDL_CreateWindow("CHIP-8 Debugger", 
                              SDL_WINDOWPOS_UNDEFINED, 
                              SDL_WINDOWPOS_UNDEFINED,
                              DEBUG_WIDTH, DEBUG_HEIGHT, 
                              SDL_WINDOW_SHOWN);
    
    if (!window) {
        printf("window creation failed: %s\n", SDL_GetError());
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("renderer creation failed: %s\n", SDL_GetError());
        return false;
    }
    
    font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf", 14);
    if (!font) {
        printf("font loading failed: %s\n", TTF_GetError());
        printf("install DejaVu fonts or modify the font path (debug.cpp:46)\n");
        return false;
    }
    
    fontSmall = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 11);
    if (!fontSmall) {
        fontSmall = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSansMono.ttf", 11);
    }
    
    return true;
}

void Debugger::cleanup() {
    if (font) TTF_CloseFont(font);
    if (fontSmall) TTF_CloseFont(fontSmall);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
}

void Debugger::drawText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* f) {
    if (!f) f = font;
    if (!f) return;
    
    SDL_Surface* surface = TTF_RenderText_Blended(f, text.c_str(), color);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        SDL_Rect destRect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

void Debugger::drawRect(int x, int y, int w, int h, SDL_Color color, bool filled) {
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    if (filled) {
        SDL_RenderFillRect(renderer, &rect);
    } else {
        SDL_RenderDrawRect(renderer, &rect);
    }
}

void Debugger::drawPanel(int x, int y, int w, int h, const std::string& title) {
    drawRect(x, y, w, h, panelColor, true);
    
    drawRect(x, y, w, h, highlightColor, false);
    
    drawRect(x, y, w, 25, {40, 40, 60, 255}, true);
    drawText(title, x + 10, y + 5, highlightColor);
}

std::string Debugger::formatHex(uint16_t value, int width) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setw(width) << std::setfill('0') << value;
    return ss.str();
}

std::string Debugger::formatBinary(uint8_t value) {
    std::stringstream ss;
    for (int i = 7; i >= 0; i--) {
        ss << ((value >> i) & 1);
        if (i == 4) ss << " ";
    }
    return ss.str();
}

void Debugger::renderCurrentInstruction() {
    int panelX = 10, panelY = 10;
    int panelW = DEBUG_WIDTH - 20;
    int panelH = 80;
    
    drawPanel(panelX, panelY, panelW, panelH, "CURRENT INSTRUCTION");
    
    int textY = panelY + 35;
    
    std::string pcText = "PC: " + formatHex(cpu->pc, 4);
    drawText(pcText, panelX + 15, textY, accentColor);
    
    std::string opcodeText = "OP: " + formatHex(cpu->opcode, 4);
    drawText(opcodeText, panelX + 150, textY, accentColor);
    
    textY += 20;
    std::string iText = "I:  " + formatHex(cpu->I, 4);
    drawText(iText, panelX + 15, textY, successColor);
    
    std::string spText = "SP: " + formatHex(cpu->sp, 2);
    drawText(spText, panelX + 150, textY, successColor);
}

void Debugger::renderRegisters() {
    int panelX = 10, panelY = 100;
    int panelW = DEBUG_WIDTH - 20;
    int panelH = 180;
    
    drawPanel(panelX, panelY, panelW, panelH, "REGISTERS (V0-VF)");
    
    int textY = panelY + 35;
    int col1X = panelX + 15;
    int col2X = panelX + 200;
    
    for (int i = 0; i < 16; i++) {
        int x = (i < 8) ? col1X : col2X;
        int y = textY + (i % 8) * 17;
        
        std::stringstream ss;
        ss << "V" << std::hex << std::uppercase << i << ": " 
           << formatHex(cpu->vX[i], 2) 
           << " [" << std::dec << std::setw(3) << (int)cpu->vX[i] << "]";
        
        SDL_Color color = (i == 0xF) ? accentColor : textColor;
        drawText(ss.str(), x, y, color, fontSmall);
    }
}

void Debugger::renderStack() {
    int panelX = 10, panelY = 290;
    int panelW = DEBUG_WIDTH - 20;
    int panelH = 150;
    
    drawPanel(panelX, panelY, panelW, panelH, "STACK");
    
    int textY = panelY + 35;
    
    if (cpu->sp == 0) {
        drawText("Stack Empty", panelX + 15, textY, textColor);
    } else {
        for (int i = 0; i < std::min((int)cpu->sp, 6); i++) {
            std::stringstream ss;
            ss << "[" << i << "] " << formatHex(cpu->stack[i], 4);
            
            SDL_Color color = (i == cpu->sp - 1) ? accentColor : textColor;
            drawText(ss.str(), panelX + 15, textY + i * 18, color, fontSmall);
        }
    }
}

void Debugger::renderTimers() {
    int panelX = 10, panelY = 450;
    int panelW = DEBUG_WIDTH - 20;
    int panelH = 80;
    
    drawPanel(panelX, panelY, panelW, panelH, "TIMERS");
    
    int textY = panelY + 35;
    
    std::string delayText = "Delay:  " + formatHex(cpu->delay_timer, 2) + " [" + std::to_string((int)cpu->delay_timer) + "]";
    drawText(delayText, panelX + 15, textY, textColor);
    
    std::string soundText = "Sound:  " + formatHex(cpu->sound_timer, 2) + " [" + std::to_string((int)cpu->sound_timer) + "]";
    drawText(soundText, panelX + 15, textY + 20, textColor);
}

void Debugger::renderMemory() {
    int panelX = 10, panelY = 540;
    int panelW = DEBUG_WIDTH - 20;
    int panelH = 150;
    
    drawPanel(panelX, panelY, panelW, panelH, "MEMORY VIEW");
    
    int textY = panelY + 35;
    
    int startAddr = std::max(0, (int)cpu->pc - 8);
    
    for (int i = 0; i < 6; i++) {
        int addr = startAddr + i * 2;
        if (addr >= 4096) break;
        
        uint16_t instruction = (memory->getCell(addr) << 8) | memory->getCell(addr + 1);
        
        std::stringstream ss;
        ss << formatHex(addr, 4) << ": " << formatHex(instruction, 4);
        
        SDL_Color color = (addr == cpu->pc) ? accentColor : textColor;
        drawText(ss.str(), panelX + 15, textY + i * 18, color, fontSmall);
    }
}

void Debugger::renderControls() {
    int panelX = 10, panelY = 700;
    int panelW = DEBUG_WIDTH - 20;
    int panelH = 55;
    
    drawPanel(panelX, panelY, panelW, panelH, "CONTROLS");
    
    std::string status = stepMode ? "MODE: STEP" : "MODE: RUN";
    SDL_Color statusColor = stepMode ? accentColor : successColor;
    drawText(status, panelX + 15, panelY + 30, statusColor);
    
    drawText("F1:Step F2:Run F3:Pause", panelX + 150, panelY + 30, textColor, fontSmall);
}

void Debugger::render() {
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderClear(renderer);
    
    renderCurrentInstruction();
    renderRegisters();
    renderStack();
    renderTimers();
    renderMemory();
    renderControls();
    
    SDL_RenderPresent(renderer);
}

void Debugger::handleInput(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_F1:
                // Step mode - execute one instruction
                stepMode = true;
                running = true;
                break;
            case SDLK_F2:
                // Run mode - continuous execution
                stepMode = false;
                running = true;
                break;
            case SDLK_F3:
                // Pause
                stepMode = true;
                running = false;
                break;
            case SDLK_UP:
                memoryViewStart = std::max(0, memoryViewStart - 2);
                break;
            case SDLK_DOWN:
                memoryViewStart = std::min(4094, memoryViewStart + 2);
                break;
        }
    }
}
