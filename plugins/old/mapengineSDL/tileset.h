#ifndef MAPENGINE_TILE_H
#define MAPENGINE_TILE_H

#include "../common/plugin.h"

class TS_Tileset {
public:
    int numtiles;
    SDL_Surface **tiles;
    unsigned short width, height;
};


#endif
