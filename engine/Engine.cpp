//
// Created by alex on 10.12.2021.
//

#include "Engine.h"

bool Engine::init(std::wstring wtitle, int height, int width) {
    char * title = new char[wtitle.size()];
    std::wcstombs(title, wtitle.c_str(), 11);
    return Engine::init(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, height, width, SDL_WINDOW_SHOWN);
}

bool Engine::init(const char* title, int xpos, int ypos, int height, int width, Uint32 flags) {
    // initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::wcout << L"SDL2 could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false; // sdl could not initialize
    }

    Engine::pWindow = SDL_CreateWindow(title, xpos, ypos, height, width, flags);
    if (Engine::pWindow == nullptr) {
        std::wcout << L"Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false; // window could not create
    }
    // if the window creation succeeded create our renderer
    Engine::pRenderer = SDL_CreateRenderer(Engine::pWindow, -1, 0);
    if (Engine::pRenderer == nullptr) {
        std::wcout << L"Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false; // renderer could not initialize
    }
    std::wcout << L"renderer creation success" << std::endl;
    Engine::bRunning = true;
    return true;
}

void Engine::render() {
    // set to black
    SDL_SetRenderDrawColor(Engine::pRenderer, 255, 0, 0, 255);

    SDL_RenderClear(pRenderer); // clear the renderer to the draw color

    SDL_RenderPresent(pRenderer); // draw to the screen
}

void Engine::handleEvents() {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT: // Close button
            bRunning = false;
            break;
        case SDL_KEYDOWN: // Press any key
            if (event.key.keysym.sym == SDLK_ESCAPE) { // press ESC
                bRunning = false;
                break;
            }
        default:
            break;
        }
    }
}

void Engine::update() {
    currentFrame = int(((SDL_GetTicks() / 100) % 6));
    //Update the surface
    SDL_UpdateWindowSurface(Engine::pWindow);
}

void Engine::clean() {
    std::cout << "cleaning game\n";
    //Destroy window
    SDL_DestroyWindow(pWindow);
    SDL_DestroyRenderer(pRenderer);
    //Quit SDL subsystems
    SDL_Quit();
}

bool Engine::isRunning() { return Engine::bRunning; }

