#include "tileset.h"

static const int maxVersion = 1;

TS_Tile::TS_Tile(uint32_t *pixels, unsigned short w, unsigned short h, unsigned short d, unsigned short nexttile, bool freePixels){

    width = w;
    height = h;
    delay = d;
    nextTileIndex = nexttile;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    empty = true;

    for(int i = 0; i<w*h; i++){
        if((pixels[i]&0xff000000)!=0){
            empty = false;
            break;
        }
    }

    obstructionType = 0;

    if(freePixels){
        free(pixels);
    }
}

TS_Tile::TS_Tile(uint32_t *pixels, unsigned short w, unsigned short h, unsigned short d, unsigned short nexttile, bool freePixels, unsigned char *obstruct){

    width = w;
    height = h;
    delay = d;
    nextTileIndex = nexttile;
    GLuint *Ttexture = (GLuint *)malloc(sizeof(GLuint));
    glGenTextures(1, Ttexture);

    texture = *Ttexture;

    free(Ttexture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    obstructionType = 1;

    obstructionData = (void *)obstruct;

    if(freePixels){
        free(pixels);
    }
}

TS_Tile::~TS_Tile(){
    glDeleteTextures(1, &texture);
}

TS_TileSet::TS_TileSet(SDL_RWops *rwops, bool closeout){
    unsigned char sig[5];    //must be '.rts'...but actually not when using an rwops.
    unsigned short version[1];      //must be 1
    unsigned short numTiles[1];
    unsigned short tWidth[1];
    unsigned short tHeight[1];
    unsigned short tileBPP[1];      //must be 32
    unsigned char compressed[1];    //must be 0
    unsigned char obstructions[1];

    SDL_RWread(rwops, sig, 1, 4);

    sig[4] = '\0';

    SDL_RWread(rwops, version, 2, 1);

    if(*version>maxVersion){
        printf("[" PLUGINNAME "] TS_Tileset(rwops) Warning: Tileset is of version %i, only tilesets of version %i or lower are supported.\n", *version, maxVersion);
    }

    SDL_RWread(rwops, numTiles, 2, 1);

    if(*numTiles==0){
        printf("[" PLUGINNAME "] TS_Tileset(rwops) Warning: Tileset has no tiles.\n");
    }

    tiles.reserve(*numTiles);

    SDL_RWread(rwops, tWidth, 2, 1);
    SDL_RWread(rwops, tHeight, 2, 1);

    tileWidth = *tWidth;
    tileHeight= *tHeight;

    SDL_RWread(rwops, tileBPP, 2, 1);

    if(*tileBPP!=32){
        printf("[" PLUGINNAME "] TS_Tileset(rwops) Warning: Tileset has irregular BPP of %i. Crashes will almost certainly follow.\n", *tileBPP);
    }

    SDL_RWread(rwops, compressed, 1, 1);
    SDL_RWread(rwops, obstructions, 1, 1);

	SDL_RWseek(rwops, 240, SEEK_CUR);

    unsigned char *obstructionData = (unsigned char *)calloc((*tWidth)*(*tHeight), 1);

    uint32_t **pixelList = (uint32_t**)calloc(*numTiles, sizeof(uint32_t*));

    for(int i = 0; i<*numTiles; i++){
        pixelList[i] = (uint32_t *)calloc((*tWidth)*(*tHeight), sizeof(uint32_t));
        SDL_RWread(rwops, pixelList[i], (*tWidth)*(*tHeight), sizeof(uint32_t));
    }

    for(int i = 0; i<*numTiles; i++){
        unsigned char obsolete[1];
        unsigned char animated[1];
        unsigned short nextTile[1];
        unsigned short delay[1];
        unsigned char obstructionType[1];
        unsigned short numSegments[1];

        SDL_RWread(rwops, obsolete, 1, 1);
        SDL_RWread(rwops, animated, 1, 1);
        SDL_RWread(rwops, nextTile, 2, 1);
        if(*nextTile>*numTiles){
            printf("[" PLUGINNAME "] TS_Tileset(rwops) Warning: Tileset has animated tile with next index beyond number of tiles. Crashes will almost certainly follow.\n");
        }

        SDL_RWread(rwops, delay, 2, 1);

        SDL_RWread(rwops, obsolete, 1, 1);

        SDL_RWread(rwops, obstructionType, 1, 1);

        SDL_RWread(rwops, numSegments, 2, 1);

        SDL_RWseek(rwops, 22, SEEK_CUR);


        if((*obstructionType)==1){
            SDL_RWread(rwops, obstructionData, (*tWidth)*(*tHeight), 1);
            tiles.push_back(new TS_Tile(pixelList[i], *tWidth, *tHeight, ((*animated)?(*delay):0), *nextTile, false, obstructionData));
        }
        else if((*obstructionType)==2){
            uint64_t reserved[1];
            for(int e = 0; e < *numSegments; e++){
                SDL_RWread(rwops, reserved, 4, 2);
            }
            tiles.push_back(new TS_Tile(pixelList[i], *tWidth, *tHeight, ((*animated)?(*delay):0), *nextTile, false));
        }
        else{
            tiles.push_back(new TS_Tile(pixelList[i], *tWidth, *tHeight, ((*animated)?(*delay):0), *nextTile, false));
        }
        free(pixelList[i]);
    }

    free(obstructionData);
    free(pixelList);

    if(closeout){
        SDL_RWclose(rwops);
    }
}
