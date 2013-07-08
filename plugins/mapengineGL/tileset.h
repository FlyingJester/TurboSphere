#ifndef MAPENGINE_TS_HEAD
#define MAPENGINE_TS_HEAD

#include "mapengine.h"
#include <vector>

class TS_Tile{
public:
    TS_Tile(uint32_t *pixels, unsigned short w, unsigned short h, unsigned short, unsigned short, bool freePixels);
    TS_Tile(uint32_t *pixels, unsigned short w, unsigned short h, unsigned short, unsigned short, bool freePixels, unsigned char *obstructionData);
    ~TS_Tile();
    bool empty;
    TS_Texture texture;
    int width;
    int height;
    int delay;
    int nextTileIndex;
    int obstructionType;
    void *obstructionData;

    TS_Tile *nextTile;
};

class TS_TileSet{
public:

    TS_TileSet(const char *filename);
    TS_TileSet(SDL_RWops *rwops, bool closeout);

    std::vector<TS_Tile*> tiles;

    GLuint *tileTextureBuffer;

    int bufferCreated;

    int tileWidth;
    int tileHeight;
};


#endif
