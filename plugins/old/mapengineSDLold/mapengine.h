
#ifndef MAPENGINE_HEAD
#define MAPENGINE_HEAD
#include <vector>
#include <string>
#include "SDL_V8.h"
#include "graphic.h"

extern int MapEngine_Active;

v8::Handle<v8::Value> ExitMapEngine(const v8::Arguments& args);

v8::Handle<v8::Value> IsMapEngineRunning(const v8::Arguments& args);

v8::Handle<v8::Value> MapEngine(const v8::Arguments& args);

class TS_TileInfo{
public:
    TS_TileInfo(bool, int, int, bool, int, vector<char>); //obs type 1
    TS_TileInfo(bool, int, int, bool, int, vector<TS_Segment>); //obs type 2
    TS_TileInfo();                                  //not animated,  obs type 0
    bool animated;
    int  anim_nexttile;
    int  anim_delay;
    bool obs_exists;
    int  obs_type;
    vector<TS_Segment>  obs_segments;
    vector<char>        obs_map;
};

class TS_Tileset{
public:

	TS_Tileset(void);
	TS_Tileset(string);
	TS_Tileset(vector<SDL_Surface*>);
	TS_Tileset(string, string);
	TS_Tileset(vector<SDL_Surface*>, string);
	~TS_Tileset();

	string name;
	vector<SDL_Surface*> tiles;
	vector<TS_TileInfo> tiledata;
	unsigned int width;
	unsigned int height;

	void PushTile(vector<SDL_Surface*>);
	void PushTile(SDL_Surface*);
	void concatTileset(TS_Tileset);
};

class TS_MapLayer{
public:
			// width,			height,			flags,	paralX, paralY, scrollX, scrollY, nsegments, reflective, name
	TS_MapLayer(unsigned int, unsigned int, unsigned int, float, float, float, float, int, int, bool);
	bool parallax, visible, reflective;
	unsigned int width, height;
	float parallaxX, parallaxY, scrollX, scrollY;
	int tileset;
	vector<unsigned int> tiles;
	vector<unsigned int> segments;

private:
	unsigned int flags;
};

class TS_Map{
public:
	TS_Map(int, int, unsigned int, unsigned int, unsigned int);
		int startX, startY;
		unsigned int numLayers, startLayer, startDirection;
		v8::Handle<v8::Script> scripts[9];
		std::vector<TS_MapLayer> Layers;
		std::string tilesetname;
		SDL_Surface *field;
		TS_Tileset *tileset;
};

class TS_Entity{
public:
    TS_Entity(void);
    ~TS_Entity();


};

#endif
