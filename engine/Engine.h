//
// Created by alex on 10.12.2021.
//

#ifndef NBODYSIM_ENGINE_H
#define NBODYSIM_ENGINE_H

#include <SDL2/SDL.h>
#include <iostream>

class Engine {
public:

    bool init(std::wstring wtitle, int height, int width);
    bool init(const char* title, int xpos, int ypos, int height, int width, Uint32 flags);
    void render();
    void update();
    void handleEvents();
    void clean();
    bool isRunning();

private:
    int currentFrame;
    bool bRunning;
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
};

#endif //NBODYSIM_ENGINE_H

