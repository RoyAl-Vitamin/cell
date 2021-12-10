//
// Created by alex on 10.12.2021.
//

#include <locale>
#include <cstdlib>
#include "engine/Engine.h"
#include <SDL2/SDL.h>


#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

const int HEIGHT = 640;
const int WIDTH = 480;

const int FPS = 60;
const int DELAY_TIME = 1000.0f / FPS;

int main(int argc, char* argv[]) {
    unsigned int frameStart, frameTime;
    auto pEngine = std::make_unique<Engine>();

    pEngine->init(L"Cell", HEIGHT, WIDTH);

    while(pEngine->isRunning()) {
        frameStart = SDL_GetTicks();

        pEngine->handleEvents();
        pEngine->update();
        pEngine->render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime< DELAY_TIME) {
            SDL_Delay((int)(DELAY_TIME - frameTime));
        }
    }
    pEngine->clean();
    return 0;
}
