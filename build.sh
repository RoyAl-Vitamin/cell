#!/bin/bash
#source /home/alex/Documents/dev/emsdk/emsdk_env.sh
em++ main.cpp engine/Engine.cpp engine/cell/Cell.cpp -s USE_SDL=2 -o main.html
