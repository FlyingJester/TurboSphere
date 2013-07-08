#ifndef MAPENGINE_MP_HEAD
#define MAPENGINE_MP_HEAD
#include "mapengine.h"
#include "tileset.h"
#include "person.h"
#include <vector>

struct TS_MapDrawInstruction {
    TS_MapDrawInstruction(int x, int y, int w, int h, int htile, int vtile, int tw, int th, const TS_Color *color, TS_Texture tex);
    ~TS_MapDrawInstruction();
    GLuint VertexBuffer;
    GLuint ColorBuffer;
    GLuint TextureCoordBuffer;
    TS_Texture texture;
    int x;
    int y;
    int width;
    int height;
    int htile;
    int vtile;
    int tileWidth;
    int tileHeight;
    const TS_Color *color;

    GLint *vertexDataFixed;
    GLint *texcoordDataFixed;
    GLint *colorDataFixed;

    void DrawAsArray(TS_Camera camera);
    void BaseDraw(void);
};
/*
Global event scripts:
SCRIPT_ON_ENTER_MAP
SCRIPT_ON_LEAVE_MAP
SCRIPT_ON_LEAVE_MAP_NORTH
SCRIPT_ON_LEAVE_MAP_EAST
SCRIPT_ON_LEAVE_MAP_SOUTH
SCRIPT_ON_LEAVE_MAP_WEST
*/
class TS_GlobalMapData{
public:
    TS_GlobalMapData(void);
    v8::Handle<v8::Script> updateScript;
    v8::Handle<v8::Script> renderScript;

    v8::Handle<v8::Script> enterMapScript;
    v8::Handle<v8::Script> leaveMapScript;
    v8::Handle<v8::Script> leaveMapNorthScript;
    v8::Handle<v8::Script> leaveMapEastScript;
    v8::Handle<v8::Script> leaveMapSouthScript;
    v8::Handle<v8::Script> leaveMapWestScript;
};

TS_GlobalMapData *GetGlobalMapData(void);
void InitGlobalMapScripts(void);

class TS_MapLayer{
public:
    ~TS_MapLayer();
    TS_MapLayer(char *, unsigned short, unsigned short);
    std::vector<unsigned short> tiles;
    std::vector<unsigned short> RLEdata;
    std::vector<TS_MapDrawInstruction *> DrawingInstructions;

    std::vector<TS_Segment> obstructions;

    bool NeedsUpdate;

    int width;
    int height;
    const char *name;
};

class TS_MapZone{
public:
    TS_MapZone(short _x, short _y, short _w, short _h, short lay, short steps, const char * func, const char *mapname);
    ~TS_MapZone();

    short layer;
    short x;
    short y;
    short w;
    short h;
    short activateSteps;
    const char *function;

    v8::Handle<v8::Script> script;

    void runScript(void);

private:
    const char *scriptname;

};

class TS_Map{
public:
    TS_Map(void);
    TS_Map(const char *);
    ~TS_Map();
    TS_TileSet *tileset;
    std::vector<TS_MapLayer> layers;
    std::vector<TS_MapZone> zones;
    //TS_TileSet *embeddedTileSet;
    char **strings;
    void Calculate(void);
    void unoptDraw(TS_Camera cam);
    void ArrayDraw(TS_Camera cam);
    void BufferDraw(TS_Camera cam);
    void Update(TS_Camera cam);
    void Render(TS_Camera cam);
    void postRender(void);
};

TS_Map *GetCurrentMap(void);
void ActivateMapEngine(void);
void DeactivateMapEngine(void);
void ResetMapEngine(void);
void TS_MapFinalizer(V8FINALIZERARGS);

void TS_MapEngine(TS_Map* map);

v8Function IsMapEngineRunning(V8ARGS);
v8Function SetRenderScript(V8ARGS);
v8Function SetUpdateScript(V8ARGS);
v8Function ChangeMap(V8ARGS);
v8Function ExitMapEngine(V8ARGS);

#endif
