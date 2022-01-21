#!/bin/bash
#source /home/alex/Documents/dev/emsdk/emsdk_env.sh
em++ main.cpp engine/Engine.cpp engine/cell/Cell.cpp engine/cell/Status.cpp -s USE_SDL=2 -o index.html
