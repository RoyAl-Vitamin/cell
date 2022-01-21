//
// Created by alex on 10.12.2021.
//

#ifndef NBODYSIM_CELL_H
#define NBODYSIM_CELL_H

#include <SDL2/SDL.h>
#include "Status.cpp"
#include <iostream>

class Cell {
public:
    void setStatus(Status status);
    Status getStatus();
    size_t getIterAlive();
    size_t incIterAlive();
    SDL_Color getColor();
    static constexpr SDL_Color DEAD_COLOR = {31, 31, 77, 255};
private:
    const size_t MAX_ITER_ALIVE = 4;
    size_t iterAlive; // Количество итераций, при которых клетка была в состоянии живой
    Status status;
    static constexpr SDL_Color ALIVE_COLOR_1 = {89, 87, 217, 255};
    static constexpr SDL_Color ALIVE_COLOR_2 = {84, 82, 204, 255};
    static constexpr SDL_Color ALIVE_COLOR_3 = {73, 71, 179, 255};
    static constexpr SDL_Color ALIVE_COLOR_4 = {61, 59, 148, 255};
};

#endif //NBODYSIM_CELL_H
