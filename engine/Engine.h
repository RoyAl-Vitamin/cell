//
// Created by alex on 10.12.2021.
//

#ifndef NBODYSIM_ENGINE_H
#define NBODYSIM_ENGINE_H

#include <SDL2/SDL.h>
#include <iostream>
#include <random>
#include "cell/Cell.h"

class Engine {
public:

    bool init(std::wstring wtitle, int width, int height);
    bool init(const char* title, int xpos, int ypos, int width, int height, Uint32 flags);
    void render();
    void update();
    void handleEvents();
    void clean();
    bool isRunning();

private:

    void renderNet();
    void renderField();
    void renderUserInput();
    void calcField();
    void copyField();
    void countAliveCurrentField();

    const int hCell = 8;
    const int wCell = 8;
    Cell **prevField;
    Cell **currField;
    Cell **userField;
    int currentFrame;
    bool bRunning;
    bool bUserInput;
    int height, width;
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    static constexpr SDL_Color DEAD_COLOR = Cell::DEAD_COLOR;
    static constexpr SDL_Color BORDER_COLOR = {21, 20, 77, 255};
    static constexpr SDL_Color USER_INPUT_COLOR = {255, 255, 255, 255};
};

#endif //NBODYSIM_ENGINE_H

