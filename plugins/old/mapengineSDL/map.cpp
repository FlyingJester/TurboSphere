#include "map.h"
#include "tileset.h"
#include "../graphicSDL/graphic.h"



TS_Map::TS_Map(int start_x_, int start_y_, std::vector<TS_mapLayer> layers_){

    start_x = start_x_;
    start_y = start_y_;
    layers = layers_;

}

void TS_Map::draw(SDL_Surface *dest, SDL_Rect *destrect){

    TS_SurfaceShowSurface(image, dest, destrect->x, destrect->y);

}

void TS_Map::render(){

    for(int i = 0; i<layers.size(); i++){
        layers[i].render(tileset, embedTileset);
        //fancy logic to eliminate unnecessary blitting to follow:
        layers[i].draw(image, NULL);
    }

}

void TS_mapLayer::draw(SDL_Surface *dest, SDL_Rect *destrect){
    SDL_BlitSurface(image, NULL, dest, destrect);
}

void TS_mapLayer::render(TS_Tileset *tileset, TS_Tileset *embedTileset){

    SDL_Rect dest = {0, 0, tileset->width, tileset->height};

    for(int x = 0; x<w; x++){
        for(int y = 0; y<h; y++){
            int tileid = tiles[x+(w*y)];
            if(tileid<embedTileset->numtiles){
                SDL_BlitSurface(tileset->tiles[tileid], NULL, image, &dest);
            }
            else{
                SDL_BlitSurface(embedTileset->tiles[tileid-embedTileset->numtiles], NULL, image, &dest);
            }
            dest.y+=tileset->height;
        }
        dest.x+=tileset->width;
    }
}
