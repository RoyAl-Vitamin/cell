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
private:
    Status status;
};

#endif //NBODYSIM_CELL_H
