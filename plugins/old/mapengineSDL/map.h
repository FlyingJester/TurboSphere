#ifndef MAPENGINE_MAP_H
#define MAPENGINE_MAP_H
#define PLUGINNAME "mapengineSDL"
#include "../common/plugin.h"
#include <vector>
#include "tileset.h"

class TS_mapLayer;

class TS_Map {
public:

    TS_Map(int start_x_, int start_y_, std::vector<TS_mapLayer> layers_);
    ~TS_Map();
    int start_x, start_y;
    int numlayers;
    std::vector<TS_mapLayer> layers;
    TS_Tileset *tileset;
    TS_Tileset *embedTileset;

    SDL_Surface *image;

    void render(void);
    void draw(SDL_Surface *dest, SDL_Rect *destrect);


};

class TS_mapLayer{
public:
    TS_mapLayer();
    ~TS_mapLayer();
    SDL_Surface *image;

    bool parallax;
    int x_par, y_par;

    int w, h;

    void render(TS_Tileset *tileset, TS_Tileset *embedTileset); //renders to its own image;
    void draw(SDL_Surface *dest, SDL_Rect *destrect);
private:
    int * tiles;
};

#endif
