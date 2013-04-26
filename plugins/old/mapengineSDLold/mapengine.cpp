using namespace std;

#include "mapengine.h"
#include "graphic.h"

TS_Tileset::TS_Tileset(){

}

TS_Tileset::~TS_Tileset(){

}

TS_Entity::TS_Entity(){

}

TS_Entity::~TS_Entity(){

}

/*
    bool animated;
    int  anim_nexttile;
    int  anim_delay;
    bool obs_exists;
    int  obs_type;
    vector<TS_Segment> obs_segments;
    char obs_map[];
*/

TS_TileInfo::TS_TileInfo(bool _animated, int _anim_nexttile, int _anim_delay, bool _obs_exists, int _obs_type, vector<char> _obs_map){ //obs type 1
    animated        = _animated;
    anim_nexttile   = _anim_nexttile;
    anim_delay      = _anim_delay;
    obs_exists      = _obs_exists;
    obs_type        = _obs_type;
    obs_map         = _obs_map;
}

TS_TileInfo::TS_TileInfo(bool _animated, int _anim_nexttile, int _anim_delay, bool _obs_exists, int _obs_type, vector<TS_Segment> _obs_segments){ //obs type 2
    animated        = _animated;
    anim_nexttile   = _anim_nexttile;
    anim_delay      = _anim_delay;
    obs_exists      = _obs_exists;
    obs_type        = _obs_type;
    obs_segments    = _obs_segments;
}

TS_TileInfo::TS_TileInfo(){ //full config
    animated        = false;
    anim_nexttile   = 0;
    anim_delay      = 0;
    obs_exists      = false;
    obs_type        = 0;
}

v8::Handle<v8::Value> ExitMapEngine(const v8::Arguments& args){
    return v8::Undefined();
}

v8::Handle<v8::Value> IsMapEngineRunning(const v8::Arguments& args){
    return v8::False();
}

v8::Handle<v8::Value> MapEngine(const v8::Arguments& args){
    return v8::Undefined();
}

void readTileset(TS_Tileset *tileset, SDL_RWops *tilesetfile){

    if(!tilesetfile){
		printf("Could not open file %i\n", tilesetfile->type);   //the file should be checked BEFORE passing it to the constructor!
        return;
	}

	unsigned char  sig[5]    = "    ";
    unsigned short version[1];
	unsigned short numtiles[1];
	unsigned short tilewidth[1];
	unsigned short tileheight[1];
	unsigned short tilebpp[1];
	unsigned char  compression[1];
    unsigned char  hasobstructions[2];
    unsigned char  obsolete[2];

	unsigned char  ti_animated[1];
	unsigned short ti_nexttile[1];
    unsigned short ti_delay[1];
	unsigned char  ti_obstructed[1];
    unsigned short ti_numsegments[1];

    //unsigned short ti_segments[4];

    size_t pos = 0;

	//sig
    pos+=SDL_RWread(tilesetfile, sig, 1,4);

    //version
    pos+=SDL_RWread(tilesetfile, version, 1,2);

    //num_tiles
    pos+=SDL_RWread(tilesetfile, numtiles, 1,2);

    //tile_width
    pos+=SDL_RWread(tilesetfile, tilewidth, 1,2);

    //tile_height
    pos+=SDL_RWread(tilesetfile, tileheight, 1,2);

    //tile_bpp
    pos+=SDL_RWread(tilesetfile, tilebpp, 1,2);

    //compression -- I would love to get this working.
    pos+=SDL_RWread(tilesetfile, compression, 1,1);

    //hasobstructions
    pos+=SDL_RWread(tilesetfile, hasobstructions, 1,1);


    //prepare TS_Info and tile surfaces to be read into.
    const int cnumtile      = (int)numtiles[0];
    const int ctilewidth    = (int)(*tilewidth);
    const int ctileheight   = (int)(*tileheight);
    const int tilepixsize   = ctilewidth*ctileheight;

    vector<SDL_Surface*>tiles;

    vector<TS_TileInfo*> tileinfo;

	SDL_RWseek(tilesetfile, 256-pos, SEEK_CUR);
    for(int i = 0; i<cnumtile; i++){

        pos = 0;

        tiles.push_back(SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, *tilewidth, *tileheight, DEPTH, CHANNEL_MASKS));

        //Read tile data
        pos+=SDL_RWread(tilesetfile, tiles[i]->pixels, BPP, tilepixsize);

        //Read Tile footer
        //obsolete
        pos+=SDL_RWread(tilesetfile, obsolete, 1,1);

        //animated
        pos+=SDL_RWread(tilesetfile, ti_animated, 1,1);

        //next_tile
        pos+=SDL_RWread(tilesetfile, ti_nexttile, 1,2);

        //delay
        pos+=SDL_RWread(tilesetfile, ti_delay, 1,2);

        //obsolete
        pos+=SDL_RWread(tilesetfile, obsolete, 1,1);

        //obstructed
        pos+=SDL_RWread(tilesetfile, ti_obstructed, 1,1);

        //number of segments
        pos+=SDL_RWread(tilesetfile, ti_numsegments, 1,2);

        SDL_RWseek(tilesetfile, 32-pos, SEEK_CUR);

        if(*ti_obstructed==1){//a bitmap of obstructions.
            vector<char> _obs_map;
            _obs_map.resize(tilepixsize);
            for(int e = 0; e<tilepixsize; e++){
                SDL_RWread(tilesetfile, &_obs_map[e], 1, 1);
            }
            tileinfo.push_back(new TS_TileInfo((int)(*ti_animated), (int)(*ti_nexttile), (int)(*ti_animated), true, (int)(*ti_obstructed), _obs_map));
        }
        else if(*ti_numsegments>0){
            vector<TS_Segment> _obs_segments;
            _obs_segments.resize(*ti_numsegments);
            unsigned short int seg[4];
            for(int s = 0; s<*ti_numsegments; s++){
                SDL_RWread(tilesetfile, seg, 1, 4);
                _obs_segments[s] = TS_Segment(seg[0], seg[1], seg[2], seg[3]);
            }
        }

    }

    for(size_t i = 0; i<tileinfo.size(); i++){
        tileset->tiledata.push_back(*tileinfo[i]);
    }

    for(size_t i = 0; i<tiles.size(); i++){
        tileset->tiles.push_back(tiles[i]);
    }

}

SDL_Surface *drawLayer(TS_MapLayer *layer, TS_Tileset *tileset){

    const unsigned int tilewidth    = tileset->width;
    const unsigned int tileheight   = tileset->height;
    const unsigned int layerwidth   = layer->width;
    const unsigned int layerheight  = layer->height;

    SDL_Surface *surface   = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_SRCALPHA, tilewidth*layerwidth, tileheight*layerheight, DEPTH, CHANNEL_MASKS);

    SDL_Rect destrect      = {0, 0, (short unsigned int)tilewidth, (short unsigned int)tileheight};

        for(unsigned int x = 0; x<layerwidth; x++, destrect.x+=tilewidth){
            for(unsigned int y = 0; y<layerheight; y++, destrect.y+=tileheight){
                SDL_BlitSurface(tileset->tiles[layer->tiles[(layerwidth*y)+x]], NULL, surface, &destrect);
            }
        }
    return surface;
}

void TS_MapEngine(TS_Map map, TS_Point camera = TS_Point(0, 0)){

    //Load all layers.
    const int numlayers = (int)map.Layers.size();
    SDL_Surface *layerimages[numlayers];
    for(int i = 0; i<numlayers; i++){
        layerimages[i] = drawLayer(&(map.Layers[i]), map.tileset);
    }

}
