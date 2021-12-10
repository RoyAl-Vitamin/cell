//
// Created by alex on 10.12.2021.
//

#include "Cell.h"

void Cell::setStatus(Status status) {
    Cell::status = status;
}

Status Cell::getStatus() {
    return Cell::status;
}