//
// Created by alex on 10.12.2021.
//

#include "Cell.h"

void Cell::setStatus(Status status) {
    if (Status::ALIVE == status && Status::DEAD == Cell::status) {
        Cell::iterAlive = 1;
    }
    if (Status::ALIVE == Cell::status && Status::DEAD == status) {
        Cell::iterAlive = 0;
    }
    Cell::status = status;
}

Status Cell::getStatus() {
    return Cell::status;
}

size_t Cell::getIterAlive() {
    return Cell::iterAlive;
}

size_t Cell::incIterAlive() {
    if (Status::DEAD == Cell::status) {
        return 0;
    }
    if (Cell::iterAlive >= Cell::MAX_ITER_ALIVE) {
        return Cell::iterAlive;
    }
    Cell::iterAlive++;
    return Cell::iterAlive;
}

SDL_Color Cell::getColor() {
    switch (Cell::iterAlive) {
        case 0:
            return Cell::DEAD_COLOR;
        case 1:
            return Cell::ALIVE_COLOR_4;
        case 2:
            return Cell::ALIVE_COLOR_3;
        case 3:
            return Cell::ALIVE_COLOR_2;
        case 4:
            return Cell::ALIVE_COLOR_1;
        default:
            return Cell::ALIVE_COLOR_1;
    }
}