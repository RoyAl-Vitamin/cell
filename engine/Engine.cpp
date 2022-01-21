//
// Created by alex on 10.12.2021.
//

#include "Engine.h"

bool Engine::init(std::wstring wtitle, int width, int height) {
    srand(static_cast <unsigned> (time(0)));

    char * title = new char[wtitle.size()];
    std::wcstombs(title, wtitle.c_str(), 11);

    Engine::prevField = new Cell * [width / Engine::wCell];
    Engine::currField = new Cell * [width / Engine::wCell];
    Engine::userField = new Cell * [width / Engine::wCell];    
    unsigned int alive = 0, dead = 0;
    for (unsigned int i = 0; i < width / Engine::wCell; i++) {
        Engine::prevField[i] = new Cell[height / Engine::hCell];
        Engine::currField[i] = new Cell[height / Engine::hCell];
        Engine::userField[i] = new Cell[height / Engine::hCell];
        for (unsigned int j = 0; j < height / Engine::hCell; j++) {
            if (((double) rand() / (RAND_MAX)) > 0.9) {
                Engine::currField[i][j].setStatus(Status::ALIVE);
                Engine::prevField[i][j].setStatus(Status::ALIVE);
                alive++;
            } else {
                Engine::currField[i][j].setStatus(Status::DEAD);
                Engine::prevField[i][j].setStatus(Status::DEAD);
                dead++;
            }
            Engine::userField[i][j].setStatus(Status::DEAD);
        }
    }
    std::wcout << L"ALIVE == " << alive << L", DEAD == " << dead << std::endl;

    return Engine::init(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
}

bool Engine::init(const char* title, int xpos, int ypos, int width, int height, Uint32 flags) {
    // initialize SDL
    #ifdef __EMSCRIPTEN__
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::wcout << L"SDL2 could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false; // sdl could not initialize
    }
    #else
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::wcout << L"SDL2 could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false; // sdl could not initialize
    }
    #endif

    Engine::pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
    if (Engine::pWindow == nullptr) {
        std::wcout << L"Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false; // window could not create
    }
    std::wcout << L"width = " << width << L", height = " << height << std::endl;
    Engine::width = width;
    Engine::height = height;
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

    SDL_SetRenderDrawColor(Engine::pRenderer, Engine::DEAD_COLOR.r, Engine::DEAD_COLOR.g, Engine::DEAD_COLOR.b, Engine::DEAD_COLOR.a);

    SDL_RenderClear(pRenderer); // clear the renderer to the draw color

    renderNet();

    calcField();

    renderField();

    renderUserInput();

    copyField();

    SDL_RenderPresent(pRenderer); // draw to the screen
}

void Engine::handleEvents() {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: // Close button
                std::wcout << L"quit" << std::endl;
                Engine::bRunning = false;
                break;
            case SDL_KEYDOWN: // Press any key
                std::wcout << L"KEY DOWN" << std::endl;
                if (event.key.keysym.sym == SDLK_ESCAPE) { // press ESC
                    Engine::bRunning = false;
                    break;
                }
            case SDL_MOUSEMOTION: {
                if (Engine::bUserInput == false) { break; }
                if (event.button.button != SDL_BUTTON_LEFT) { break; }

                int x = event.motion.x, y = event.motion.y;
                std::wcout << L"Mouse coords [" << x << L", " << y << L"] == CELL [" << x / Engine::wCell << L", " << y / Engine::hCell << L"]" << std::endl;
                int xIndex = x / Engine::wCell, yIndex = y / Engine::hCell;
                Engine::userField[xIndex][yIndex].setStatus(Status::ALIVE);
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button != SDL_BUTTON_LEFT) { break; }

                Engine::bUserInput = true;
                std::wcout << L"MOUSE DOWN" << std::endl;
                int x, y;
                SDL_GetMouseState( &x, &y );
                std::wcout << L"Mouse coords [" << x << L", " << y << L"] == CELL [" << x / Engine::wCell << L", " << y / Engine::hCell << L"]" << std::endl;
                int xIndex = x / Engine::wCell, yIndex = y / Engine::hCell;
                Engine::userField[xIndex][yIndex].setStatus(Status::ALIVE);
                break;
            }
            case SDL_MOUSEBUTTONUP:
                if (event.button.button != SDL_BUTTON_LEFT) { break; }
                Engine::bUserInput = false;
                std::wcout << L"MOUSE UP" << std::endl;
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
    if (Engine::prevField != nullptr) {
        for (unsigned int i = 0; i < Engine::width / Engine::wCell; i++) {
            delete [] Engine::prevField[i];
        }
        delete [] Engine::prevField;
    }

    if (Engine::currField != nullptr) {
        for (unsigned int i = 0; i < Engine::width / Engine::wCell; i++) {
            delete [] Engine::currField[i];
        }
        delete [] Engine::currField;
    }

    std::cout << "cleaning game\n";
    //Destroy window
    SDL_DestroyWindow(pWindow);
    SDL_DestroyRenderer(pRenderer);
    //Quit SDL subsystems
    SDL_Quit();
}

bool Engine::isRunning() { return Engine::bRunning; }

void Engine::renderNet() {
    SDL_SetRenderDrawColor(Engine::pRenderer, Engine::BORDER_COLOR.r, Engine::BORDER_COLOR.g, Engine::BORDER_COLOR.b, Engine::BORDER_COLOR.a);

    for (int i = 0; i <= Engine::width; i += Engine::wCell) {
        SDL_RenderDrawLine(Engine::pRenderer, i, 0, i, Engine::height - 1);
        SDL_RenderDrawLine(Engine::pRenderer, i + wCell - 1, 0, i + wCell - 1, Engine::height - 1);
    }
    for (int j = 0; j < Engine::height; j += Engine::hCell) {
        SDL_RenderDrawLine(Engine::pRenderer, 0, j, Engine::width - 1, j);
        SDL_RenderDrawLine(Engine::pRenderer, 0, j + hCell - 1, Engine::width - 1, j + hCell - 1);
    }
}

void Engine::renderField() {
    int count = 0;

    for (size_t i = 0; i < Engine::width / Engine::wCell; i++) {
        for (size_t j = 0; j < Engine::height / Engine::hCell; j++) {
            if (Engine::currField[i][j].getStatus() == Status::ALIVE) {
                count++;
                Engine::currField[i][j].incIterAlive();
            }
        }
    }
    //std::wcout << L"Count ALIVE == " << count << std::endl;

    SDL_Rect * rects = new SDL_Rect[count];
    count = 0;
    for (size_t i = 0; i < Engine::width / Engine::wCell; i++) {
        for (size_t j = 0; j < Engine::height / Engine::hCell; j++) {
            if (Engine::currField[i][j].getStatus() == Status::ALIVE) {
                rects[count].x = i * Engine::wCell + 1;
                rects[count].y = j * Engine::hCell + 1;
                rects[count].w = Engine::wCell - 2;
                rects[count].h = Engine::hCell - 2;
                
                SDL_Color color = Engine::currField[i][j].getColor();
                SDL_SetRenderDrawColor(Engine::pRenderer, color.r, color.g, color.b, color.a);
                SDL_RenderFillRect(Engine::pRenderer, &rects[count]);
                count++;
            }
        }
    }
    //SDL_RenderFillRects(Engine::pRenderer, rects, count);
    delete[] rects;
}

/*
 *  |-|---------|-|
 *  |1|    2    |3|
 *  |-|---------|-|
 *  | |         | |
 *  |4|    5    |6|
 *  | |         | |
 *  |-|---------|-|
 *  |7|    8    |9|
 *  |-|---------|-|
 */
void Engine::calcField() {

    // Zone 1
    int countAliveNeighbor = 0;
    if (Engine::prevField[Engine::width / Engine::wCell - 1][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[0][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[1][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

    if (Engine::prevField[Engine::width / Engine::wCell - 1][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[1][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

    if (Engine::prevField[Engine::width / Engine::wCell - 1][1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[0][1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[1][1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

    switch (Engine::prevField[0][0].getStatus()) {
        case Status::ALIVE:
            if (countAliveNeighbor < 2 || countAliveNeighbor > 3) {
                Engine::currField[0][0].setStatus(Status::DEAD);
            }
            break;
        case Status::DEAD:
            if (countAliveNeighbor == 3) {
                Engine::currField[0][0].setStatus(Status::ALIVE);
            }
            break;
    }

    // Zone 2
    for (unsigned int i = 1; i < Engine::width / Engine::wCell - 1; i++) {
        countAliveNeighbor = 0;
        if (Engine::prevField[i - 1][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[i][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[i + 1][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

        if (Engine::prevField[i - 1][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[i + 1][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

        if (Engine::prevField[i - 1][1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[i][1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[i + 1][1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

        switch (Engine::prevField[i][0].getStatus()) {
            case Status::ALIVE:
                if (countAliveNeighbor < 2 || countAliveNeighbor > 3) {
                    Engine::currField[i][0].setStatus(Status::DEAD);
                }
                break;
            case Status::DEAD:
                if (countAliveNeighbor == 3) {
                    Engine::currField[i][0].setStatus(Status::ALIVE);
                }
                break;
        }
    }

    // Zone 3
    countAliveNeighbor = 0;
    if (Engine::prevField[Engine::width / Engine::wCell - 2][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[Engine::width / Engine::wCell - 1][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[0][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

    if (Engine::prevField[Engine::width / Engine::wCell - 2][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[0][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

    if (Engine::prevField[Engine::width / Engine::wCell - 2][1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[Engine::width / Engine::wCell - 1][1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[0][1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

    switch (Engine::prevField[Engine::width / Engine::wCell - 1][0].getStatus()) {
        case Status::ALIVE:
            if (countAliveNeighbor < 2 || countAliveNeighbor > 3) {
                Engine::currField[Engine::width / Engine::wCell - 1][0].setStatus(Status::DEAD);
            }
            break;
        case Status::DEAD:
            if (countAliveNeighbor == 3) {
                Engine::currField[Engine::width / Engine::wCell - 1][0].setStatus(Status::ALIVE);
            }
            break;
    }

    // Zone 4
    for (unsigned int j = 1; j < Engine::height / Engine::hCell - 1; j++) {
        countAliveNeighbor = 0;
        if (Engine::prevField[Engine::width / Engine::wCell - 1][j - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[0][j - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[1][j - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

        if (Engine::prevField[Engine::width / Engine::wCell - 1][j].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[1][j].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

        if (Engine::prevField[Engine::width / Engine::wCell - 1][j + 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[0][j + 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[1][j + 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

        switch (Engine::prevField[0][j].getStatus()) {
            case Status::ALIVE:
                if (countAliveNeighbor < 2 || countAliveNeighbor > 3) {
                    Engine::currField[0][j].setStatus(Status::DEAD);
                }
                break;
            case Status::DEAD:
                if (countAliveNeighbor == 3) {
                    Engine::currField[0][j].setStatus(Status::ALIVE);
                }
                break;
        }
    }

    // Zone 5
    for (unsigned int i = 1; i < Engine::width / Engine::wCell - 1; i++) {
        for (unsigned int j = 1; j < Engine::height / Engine::hCell - 1; j++) {
            countAliveNeighbor = 0;
            if (Engine::prevField[i - 1][j - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
            if (Engine::prevField[i][j - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
            if (Engine::prevField[i + 1][j - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

            if (Engine::prevField[i - 1][j].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
            if (Engine::prevField[i + 1][j].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

            if (Engine::prevField[i - 1][j + 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
            if (Engine::prevField[i][j + 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
            if (Engine::prevField[i + 1][j + 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

            // std::wcout << L"For [" << i << L", " << j << L"] alive neighbor == " << countAliveNeighbor << std::endl;
            
            switch (Engine::prevField[i][j].getStatus()) {
            case Status::ALIVE:
                if (countAliveNeighbor < 2 || countAliveNeighbor > 3) {
                    Engine::currField[i][j].setStatus(Status::DEAD);
                }
                break;
            case Status::DEAD:
                if (countAliveNeighbor == 3) {
                    Engine::currField[i][j].setStatus(Status::ALIVE);
                }
                break;
            }
        }
    }

    // Zone 6
    for (unsigned int j = 1; j < Engine::height / Engine::hCell - 1; j++) {
        countAliveNeighbor = 0;
        if (Engine::prevField[Engine::width / Engine::wCell - 2][j - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[Engine::width / Engine::wCell - 1][j - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[0][j - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

        if (Engine::prevField[Engine::width / Engine::wCell - 2][j].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[0][j].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

        if (Engine::prevField[Engine::width / Engine::wCell - 2][j + 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[Engine::width / Engine::wCell - 1][j + 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[0][j + 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

        switch (Engine::prevField[Engine::width / Engine::wCell - 1][j].getStatus()) {
            case Status::ALIVE:
                if (countAliveNeighbor < 2 || countAliveNeighbor > 3) {
                    Engine::currField[Engine::width / Engine::wCell - 1][j].setStatus(Status::DEAD);
                }
                break;
            case Status::DEAD:
                if (countAliveNeighbor == 3) {
                    Engine::currField[Engine::width / Engine::wCell - 1][j].setStatus(Status::ALIVE);
                }
                break;
        }
    }

    // Zone 7
    countAliveNeighbor = 0;
    if (Engine::prevField[Engine::width / Engine::wCell - 2][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[0][Engine::height / Engine::hCell - 2].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[1][Engine::height / Engine::hCell - 2].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

    if (Engine::prevField[Engine::width / Engine::wCell - 1][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[1][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

    if (Engine::prevField[0][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[0][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[1][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

    switch (Engine::prevField[0][Engine::height / Engine::hCell - 1].getStatus()) {
        case Status::ALIVE:
            if (countAliveNeighbor < 2 || countAliveNeighbor > 3) {
                Engine::currField[0][Engine::height / Engine::hCell - 1].setStatus(Status::DEAD);
            }
            break;
        case Status::DEAD:
            if (countAliveNeighbor == 3) {
                Engine::currField[0][Engine::height / Engine::hCell - 1].setStatus(Status::ALIVE);
            }
            break;
    }

    // Zone 8
    for (unsigned int i = 1; i < Engine::width / Engine::wCell - 1; i++) {
        countAliveNeighbor = 0;
        if (Engine::prevField[i - 1][Engine::height / Engine::hCell - 2].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[i][Engine::height / Engine::hCell - 2].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[i + 1][Engine::height / Engine::hCell - 2].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

        if (Engine::prevField[i - 1][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[i + 1][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

        if (Engine::prevField[i - 1][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[i][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
        if (Engine::prevField[i + 1][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

        switch (Engine::prevField[i][Engine::height / Engine::hCell - 1].getStatus()) {
            case Status::ALIVE:
                if (countAliveNeighbor < 2 || countAliveNeighbor > 3) {
                    Engine::currField[i][Engine::height / Engine::hCell - 1].setStatus(Status::DEAD);
                }
                break;
            case Status::DEAD:
                if (countAliveNeighbor == 3) {
                    Engine::currField[i][Engine::height / Engine::hCell - 1].setStatus(Status::ALIVE);
                }
                break;
        }
    }

    // Zone 9
    countAliveNeighbor = 0;
    if (Engine::prevField[Engine::width / Engine::wCell - 2][Engine::height / Engine::hCell - 2].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[Engine::width / Engine::wCell - 1][Engine::height / Engine::hCell - 2].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[0][2].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

    if (Engine::prevField[Engine::width / Engine::wCell - 2][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[0][Engine::height / Engine::hCell - 1].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

    if (Engine::prevField[Engine::width / Engine::wCell - 2][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[Engine::width / Engine::wCell - 1][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }
    if (Engine::prevField[0][0].getStatus() == Status::ALIVE) { countAliveNeighbor++; }

    switch (Engine::prevField[Engine::width / Engine::wCell - 1][Engine::height / Engine::hCell - 1].getStatus()) {
        case Status::ALIVE:
            if (countAliveNeighbor < 2 || countAliveNeighbor > 3) {
                Engine::currField[Engine::width / Engine::wCell - 1][Engine::height / Engine::hCell - 1].setStatus(Status::DEAD);
            }
            break;
        case Status::DEAD:
            if (countAliveNeighbor == 3) {
                Engine::currField[Engine::width / Engine::wCell - 1][Engine::height / Engine::hCell - 1].setStatus(Status::ALIVE);
            }
            break;
    }

    if (Engine::bUserInput == false) {
        for (unsigned int i = 0; i < Engine::width / Engine::wCell; i++) {
            for (unsigned int j = 0; j < Engine::height / Engine::hCell; j++) {
                if (Engine::userField[i][j].getStatus() == Status::ALIVE) {
                    Engine::currField[i][j].setStatus(Status::ALIVE);
                    Engine::userField[i][j].setStatus(Status::DEAD);
                }
            }
        }
    }
}

void Engine::renderUserInput() {
    if (Engine::bUserInput == false) { return; }

    SDL_SetRenderDrawColor(Engine::pRenderer, Engine::USER_INPUT_COLOR.r, Engine::USER_INPUT_COLOR.g, Engine::USER_INPUT_COLOR.b, Engine::USER_INPUT_COLOR.a);

    int count = 0;
    for (unsigned int i = 0; i < Engine::width / Engine::wCell; i++) {
        for (unsigned int j = 0; j < Engine::height / Engine::hCell; j++) {
            if (Engine::userField[i][j].getStatus() == Status::ALIVE) {
                count++;
            }
        }
    }
    SDL_Rect * rects = new SDL_Rect[count];
    count = 0;
    for (unsigned int i = 0; i < Engine::width / Engine::wCell; i++) {
        for (unsigned int j = 0; j < Engine::height / Engine::hCell; j++) {
            if (Engine::userField[i][j].getStatus() == Status::ALIVE) {
                rects[count].x = i * Engine::wCell + 1;
                rects[count].y = j * Engine::hCell + 1;
                rects[count].w = Engine::wCell - 2;
                rects[count].h = Engine::hCell - 2;
                count++;
            }
        }
    }
    SDL_RenderFillRects(Engine::pRenderer, rects, count);
    delete[] rects;
}

void Engine::copyField() {
    for (unsigned int i = 0; i < Engine::width / Engine::wCell; i++) {
        for (unsigned int j = 0; j < Engine::height / Engine::hCell; j++) {
            Engine::prevField[i][j].setStatus(Engine::currField[i][j].getStatus());
        }
    }
}

void Engine::countAliveCurrentField() {
    int count = 0;

    for (unsigned int i = 0; i < Engine::width / Engine::wCell; i++) {
        for (unsigned int j = 0; j < Engine::height / Engine::hCell; j++) {
            if (Engine::currField[i][j].getStatus() == Status::ALIVE) {
                count++;
            }
        }
    }
    std::wcout << L"Count alive == " << count << std::endl;
}