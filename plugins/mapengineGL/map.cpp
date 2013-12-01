
#include "mapengine.h"
#include "tileset.h"
#include "map.h"
#include "person.h"
#include <cstring>
#include <assert.h>
#include "../../common/dlopenwrap.h"

#ifdef _WIN32
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

static fhandle SDLGLhandle;

void (*FlipScreenDL)(void);

TS_MapEngineInstance *TS_GetGlobalMapEngineInstance(){
    static TS_MapEngineInstance *globalInstance = (TS_MapEngineInstance*)calloc(sizeof(TS_MapEngineInstance), 1);
    return globalInstance;
}

static TS_Map *nextMap = NULL;
static bool doneWithCurrentMap = false;

void InitMapMain(void){

    #ifdef _WIN32
		DWORD error;
	    SDLGLhandle = LoadLibrary("./plugin/SDL_GL_threaded.dll");
	    if(SDLGLhandle==NULL){
            SDLGLhandle = LoadLibrary("./plugin/SDL_GL.dll");
	    }
        if(SDLGLhandle!=NULL) {
				DLOPENFUNCTION(void(*)(void), FlipScreenDL, SDLGLhandle, "FlipScreen", 0, 0, exit(0xDD));
        }
        else{
			exit(0xD9);
        }
    #else

        char *error;
        SDLGLhandle = dlopen("./plugin/libSDL_GL_threaded.so", RTLD_GLOBAL|RTLD_NOW);
        if(SDLGLhandle==NULL)
            SDLGLhandle = dlopen("./plugin/libSDL_GL.so", RTLD_GLOBAL|RTLD_NOW);

        if(SDLGLhandle==NULL) {
            fprintf(stderr, "[" PLUGINNAME "] InitMap error: Could not open any known graphics plugins.\n");
            exit(0xFD);
        }
        else{
            //DLOPENFUNCTION(v8::Local<v8::Object>(*)(int, int, void *), TS_SDL_GL_MakeV8SurfaceHandleFromPixels, handle, "TS_SDL_GL_MakeV8SurfaceHandleFromPixels", "[" PLUGINNAME "] InitSpriteSet error: Could not load TS_SDL_GL_MakeV8SurfaceHandleFromPixels from any plugin.\n", error, exit(0xFE));
            FlipScreenDL = (void (*)(void))dlsym(SDLGLhandle, "FlipScreen");
            if (((error = dlerror()) != NULL)||(FlipScreenDL==NULL))  {
                fprintf (stderr, "[" PLUGINNAME "] InitMap error: Could not load FlipScreen from any plugin.\n\tReported error is: %s", error);
                exit(0xFE);
            }
        }
    #endif
}

void CloseMapMain(void){
    #ifdef _WIN32

    #else
        dlclose(SDLGLhandle);
    #endif
}

bool *IsMapEngineRunning(){
    static bool isRunning = false;
    return &isRunning;
}

TS_Map *GetCurrentMap(){
    static TS_Map *map = NULL;
    if(!(*(IsMapEngineRunning()))){
        return NULL;
    }
    else{
        return map;
    }
}

void ActivateMapEngine(){
    *(IsMapEngineRunning()) = true;
}

void DeactivateMepEngine(){
    *(IsMapEngineRunning()) = false;
}

TS_GlobalMapData::TS_GlobalMapData(){
    currentMap = NULL;
    nextMap = NULL;
}

TS_GlobalMapData *GetGlobalMapData(){
    static TS_GlobalMapData data;
    return &data;
}
/*
TS_MapDrawInstruction::TS_MapDrawInstruction(int _x, int _y, int _w, int _h, int _htile, int _vtile, int tw, int th, const TS_Color *_color, TS_Texture tex){
    texture = tex;
    x = _x;
    y = _y;
    width   = _w;
    height  = _h;
    htile   = _htile;
    vtile   = _vtile;
    color   = _color;
    tileWidth = tw;
    tileHeight= th;
    vertexDataFixed   = (GLint*)malloc(8*sizeof(GLint));
    texcoordDataFixed = (GLint*)calloc(8, sizeof(GLint));
    colorDataFixed    = (GLint*)malloc(16);
    vertexDataFixed[0] = (x*tileWidth);
    vertexDataFixed[1] = (y*tileHeight);
    vertexDataFixed[2] = ((x+htile)*tileWidth);
    vertexDataFixed[3] = (y*tileHeight);
    vertexDataFixed[4] = ((x+htile)*tileWidth);
    vertexDataFixed[5] = ((y+vtile)*tileHeight);
    vertexDataFixed[6] = (x*tileWidth);
    vertexDataFixed[7] = ((y+vtile)*tileHeight);

    texcoordDataFixed[2] = htile;
    texcoordDataFixed[4] = htile;
    texcoordDataFixed[5] = vtile;
    texcoordDataFixed[7] = vtile;

    memset(colorDataFixed, 0xFF, 16);

    if(color->toInt()!=0xFFFFFFFF&&(false)){
        const uint32_t colorInt = color->toInt();
        for(int i = 0; i<16; i+=4){
            memcpy((void *)(colorDataFixed+i), &colorInt, 4);
        }
    }
}

TS_MapDrawInstruction::~TS_MapDrawInstruction(){
    free(vertexDataFixed);
    free(colorDataFixed);
    free(texcoordDataFixed);
}
*/
TS_Map::~TS_Map(){
}
/*
void TS_MapDrawInstruction::DrawAsArray(TS_Camera camera){
    const GLint   vertexData[]   = {(x*tileWidth)-camera.x, (y*tileHeight)-camera.y,
        ((x+htile)*tileWidth)-camera.x, (y*tileHeight)-camera.y,
        ((x+htile)*tileWidth)-camera.x, ((y+vtile)*tileHeight)-camera.y,
        (x*tileWidth)-camera.x, ((y+vtile)*tileHeight)-camera.y};

    //const GLint   texcoordData[] = {0, 0, htile, 0, htile, vtile, 0, vtile};
    glVertexPointer(2, GL_INT, 0, vertexData);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void TS_MapDrawInstruction::BaseDraw(){

    glTexCoordPointer(2, GL_INT, 0, texcoordDataFixed);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorDataFixed);
    glVertexPointer(2, GL_INT, 0, vertexDataFixed);
    glBindTexture(GL_TEXTURE_2D, texture);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


Global event scripts:
SCRIPT_ON_ENTER_MAP
SCRIPT_ON_LEAVE_MAP
SCRIPT_ON_LEAVE_MAP_NORTH
SCRIPT_ON_LEAVE_MAP_EAST
SCRIPT_ON_LEAVE_MAP_SOUTH
SCRIPT_ON_LEAVE_MAP_WEST
*/
/*
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
*/
TS_GlobalMapData *GetGlobalMapData(void);
void InitGlobalMapScripts(void);


TS_MapLayer::TS_MapLayer(char * n, unsigned short w, unsigned short h){
    width = w;
    height= h;
    name = STRDUP(n);
    tiles.reserve(w*h);
}

TS_MapLayer::~TS_MapLayer(){
    //for(size_t i = 0; i<DrawingInstructions.size(); i++){
    //    delete DrawingInstructions[i];
    //}
    tiles.resize(0);
    free((void *)name);
}

void TS_MapLayer::Draw(void){

}

TS_MapZone::TS_MapZone(short _x, short _y, short _w, short _h, short lay, short steps, const char * func, const char * mapname){

    activateSteps = steps;
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    layer = lay;
    function = func;
    scriptname = STRDUP((string("ZONE SCRIPT ")+string(mapname)).c_str());
    script = v8::Script::Compile(v8::String::New(func), v8::String::New(scriptname));

}

TS_MapZone::~TS_MapZone(){
    script.Clear();
    free((void *)function);
    free((void *)scriptname);
}

TS_Map::TS_Map(const char *filename){
    if(filename==NULL){
        printf("[" PLUGINNAME "] Error: No map filename passed.\n");   //the file should be checked BEFORE passing it to the constructor!
        return;
    }
    SDL_RWops *mapfile;
	mapfile = SDL_RWFromFile(filename, "rb");
    if(!mapfile){
		printf("[" PLUGINNAME "] Error: Could not open file %s\n", filename);   //the file should be checked BEFORE passing it to the constructor!
        return;
	}
	unsigned char unused[8];
    unsigned short version[1];
	unsigned short numlayers[1];
	unsigned short numentities[1];
	unsigned short startx[1];
	unsigned short starty[1];
	unsigned char startlayer[1];
	unsigned char startdirection[1];
	unsigned short numstrings[1]; //must be 9
	unsigned short numzones[1];
	unsigned char sig[5]    = "    ";

    bool internalTileSet = false;

    //HEADER

	//sig
    SDL_RWread(mapfile, sig, 1, 4);

    //version
    SDL_RWread(mapfile, version, 2, 1);

    //obsolete type attribute
    SDL_RWread(mapfile, unused, 1, 1);

    //num_layers
    SDL_RWread(mapfile, numlayers, 2, 1);

    //reserved (for what? Compression?)
    SDL_RWread(mapfile, unused, 1, 1);

    //num_entities
    SDL_RWread(mapfile, numentities, 1, 1);

    //startx (in pixels? I think Sphere assumes 16x16 tiles for this)
    SDL_RWread(mapfile, startx, 2, 1);

    //starty
    SDL_RWread(mapfile, starty, 2, 1);

    //starting layer
    SDL_RWread(mapfile, startlayer, 1, 1);

    //starting direction (How are diagonals handled?)
    SDL_RWread(mapfile, startdirection, 1, 1);

    //num_string
    SDL_RWread(mapfile, numstrings, 2, 1);

    //numzones
    SDL_RWread(mapfile, numzones, 2, 1);

    //cog
    zones.reserve(*numzones);

	SDL_RWseek(mapfile, 256, SEEK_SET);
    layers.reserve(numlayers[0]);

    //STRINGS
    strings = (char **)calloc(*numstrings, sizeof(char*));
    unsigned short stringlength[1];

    //Assume strings are NOT null ternimated.

    for(int i = 0; i<*numstrings; i++){
        SDL_RWread(mapfile, stringlength, 2, 1);
        if((i==0)&&(*stringlength==0)){
            internalTileSet = true; //TODO: embed+extern
        }
        strings[i] = (char *)calloc((*stringlength)+1, sizeof(char));
        SDL_RWread(mapfile, strings[i], 1, *stringlength);
    }

    //LAYERS

    if(sizeof(float)!=4){
        printf("[" PLUGINNAME "] Warning: Internal floating point size mismatch. Undefined behaviour may occur.\n");
    }

    //unsigned short tilenum[1];
    unsigned short width[1];
    unsigned short height[1];
    unsigned short flags[1];
    float parallax_x[1]; //to be sure we have a float32?
    float parallax_y[1];
    float scrolling_x[1];
    float scrolling_y[1];
    uint32_t numsegments[1];
    unsigned char reflective[1];
    unsigned char reserved[3];

    for(int i = 0; i<*numlayers; i++){
        //LAYER HEADER
        SDL_RWread(mapfile, width,  2, 1);
        SDL_RWread(mapfile, height, 2, 1);
        SDL_RWread(mapfile, flags,  2, 1);
        SDL_RWread(mapfile, parallax_x,  4, 1);
        SDL_RWread(mapfile, parallax_y,  4, 1);
        SDL_RWread(mapfile, scrolling_x, 4, 1);
        SDL_RWread(mapfile, scrolling_y, 4, 1);
        SDL_RWread(mapfile, numsegments, 4, 1);
        SDL_RWread(mapfile, reflective,  1, 1);
        SDL_RWread(mapfile, reserved,1, 3);
        SDL_RWread(mapfile, stringlength,2, 1);
        char *layername = (char *)calloc((*stringlength)+1, sizeof(char));
        SDL_RWread(mapfile, layername, 1, *stringlength);
        TS_MapLayer layer = TS_MapLayer(layername, *width, *height);
        for(int y = 0; y<*height; y++){
            for(int x = 0; x<*width; x++){
                unsigned short temptile[1];
                SDL_RWread(mapfile, temptile, 2, 1);
                layer.tiles.push_back(*temptile);
            }
        }
        for(uint32_t e = 0; e<*numsegments; e++){
            TS_Segment obstruction;
            uint32_t obs[4];
            SDL_RWread(mapfile, obs, 4, 4);
            obstruction.x1 = obs[0];
            obstruction.y1 = obs[1];
            obstruction.x2 = obs[2];
            obstruction.y2 = obs[3];
            layer.obstructions.push_back(obstruction);
        }
        layers.push_back(layer);
    }
    for(int i = 0; i<*numentities; i++){
        unsigned char temp[16];
        SDL_RWread(mapfile, temp, 16, 1);

        SDL_RWread(mapfile, stringlength, 2, 1);
        char *entityname = (char *)calloc((*stringlength)+1, sizeof(char));
        SDL_RWread(mapfile, entityname, 1, *stringlength);
        unsigned short numscripts[1];
        SDL_RWread(mapfile, numscripts, 2, 1);

        for(int e = 0; e<*numscripts; e++){
            SDL_RWread(mapfile, stringlength, 2, 1);
            char *script = (char *)calloc((*stringlength)+1, sizeof(char));
            SDL_RWread(mapfile, script, 1, *stringlength);
        }
        SDL_RWread(mapfile, temp, 16, 1);

        //trigger function

        SDL_RWread(mapfile, stringlength, 2, 1);
        char *triggerscript = (char *)calloc((*stringlength)+1, sizeof(char));
        SDL_RWread(mapfile, triggerscript, 1, *stringlength);
        //add entity
    }
    for(int i = 0; i<*numzones; i++){
        short x1[1];
        short y1[1];
        short x2[1];
        short y2[1];
        short layer[1];
        short steps[1];
        unsigned char temp[4];

        SDL_RWread(mapfile, x1, 2, 1);
        SDL_RWread(mapfile, y1, 2, 1);
        SDL_RWread(mapfile, x2, 2, 1);
        SDL_RWread(mapfile, y2, 2, 1);
        SDL_RWread(mapfile, layer, 2, 1);
        SDL_RWread(mapfile, steps, 2, 1);

        SDL_RWread(mapfile, temp, 1, 4);

        SDL_RWread(mapfile, stringlength, 2, 1);
        char *func = (char *)calloc((*stringlength)+1, sizeof(char));
        SDL_RWread(mapfile, func, 1, *stringlength);

        zones.push_back(TS_MapZone(*x1, *y1, short((*x2)-(*x1)), short((*y2)-(*y1)), *layer, *steps, STRDUP(func), STRDUP(filename)));

    }
    if(internalTileSet){
        tileset = new TS_TileSet(mapfile, true);
    }
}
/*
void TS_Map::Calculate(){
    const TS_Color *fullmask = new TS_Color(0xFF, 0xFF, 0xFF, 0xFF);
    for(size_t l = 0; l<layers.size(); l++){
        for(int y = 0; y<layers[l].height; y++){
            unsigned short lastTile = layers[l].tiles[y*layers[l].width];
            int run = 0;
            for(int x = 0; x<layers[l].width; x++){
                if(layers[l].tiles[x+(y*layers[l].width)]!=lastTile){
                    layers[l].DrawingInstructions.push_back(new TS_MapDrawInstruction((x-1)-run, y, run, 1, run, 1, tileset->tileWidth, tileset->tileHeight, fullmask, tileset->tiles[lastTile]->texture));
                    lastTile = layers[l].tiles[x+(y*layers[l].width)];
                    run = 1;
                }
                else{
                    run++;
                }
            }
            //printf("run: %i\tscanline: %i", run, y);
            layers[l].DrawingInstructions.push_back(new TS_MapDrawInstruction((layers[l].width-1)-run, y, run, 1, run, 1, tileset->tileWidth, tileset->tileHeight, fullmask, tileset->tiles[lastTile]->texture));
        }
    }
}
*/
void TS_Map::Update(TS_Camera camera){
    SDL_PumpEvents();
}

void TS_Map::Render(TS_Camera camera){
    //static bool first = true;
    glPushMatrix();
    glTranslatef((float)camera.x, float(camera.y), 0);
    Draw();

    /*
    for(size_t l = 0; l<layers.size(); l++){
        if(first){
            printf("Layer %i had %i instructions.\t", l, layers[l].DrawingInstructions.size());
        }
        TS_Person **tempPersonList = GetPersonsOnLayer(l);
        for(size_t i = 0; i<layers[l].DrawingInstructions.size(); i++){
            layers[l].DrawingInstructions[i]->BaseDraw();
        }
        for(int i = 0; i<GetNumPersonsOnLayer(l); i++){
            tempPersonList[i]->BaseDraw();
            tempPersonList[i]->tick++;
        }
        //free((void *)tempPersonList);
    }
    if(first){
        first = false;
        printf("\n");
    }*/
    glPopMatrix();
}

void TS_Map::Draw(void){
//assume NOTHING!

//    int np = GetNumPersonsOnLayer(l);
    const GLint   texcoordData[] = {0, 0, 1, 0, 1, 1, 0, 1};
    const GLuint  colorData[]    = {
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF
    };
    glTexCoordPointer(2, GL_INT, 0, texcoordData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnable(GL_TEXTURE_2D);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_COLOR_ARRAY);
    for(size_t l = 0; l<layers.size(); l++){
        for(int y = 0; y<layers[l].height; y++){
            for(int x = 0; x<layers[l].width; x++){
                int tilenum = layers[l].tiles[x+(layers[l].width*y)];
                const GLint   vertexData[]   = {(x*tileset->tileWidth), (y*tileset->tileHeight),
                                                ((x+1)*tileset->tileWidth), (y*tileset->tileHeight),
                                                ((x+1)*tileset->tileWidth), ((y+1)*tileset->tileHeight),
                                                (x*tileset->tileWidth), ((y+1)*tileset->tileHeight)};

                glVertexPointer(2, GL_INT, 0, vertexData);
                glBindTexture(GL_TEXTURE_2D, tileset->tiles[tilenum]->texture);

                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            }
        }
        //draw Persons.
        for(int i = 0; i<personList.size(); i++){
            if(personList[i]->layer==l){
                personList[i]->BaseDraw();
            }
        }

    }
                glDisableClientState(GL_COLOR_ARRAY);
                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

v8Function IsMapEngineRunning(V8ARGS){
    if(nextMap||(!doneWithCurrentMap)){
        return v8::True();
    }
    else{
        return v8::False();
    }
}

v8Function SetRenderScript(V8ARGS){


    return v8::Undefined();
}

v8Function SetUpdateScript(V8ARGS){

    return v8::Undefined();
}

v8Function ChangeMap(V8ARGS){
    if(args.Length()<1){
        THROWERROR("[" PLUGINNAME "] ChangeMap Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    TS_Directories *TS_dirs = GetDirs();

    v8::String::Utf8Value str(args[0]);

    const char *cstr = *str;

//    int screenwidth = GetScreenWidth();
//    int screenheight = GetScreenHeight();

    TS_Map *map = new TS_Map((std::string(TS_dirs->map)+std::string(cstr)).c_str());

    if(map==NULL){
        THROWERROR(string("[" PLUGINNAME "] ChangeMap Error: Could not open map ").append(cstr).c_str());
    }
    nextMap = map;
    doneWithCurrentMap = true;
    return v8::Undefined();
}

v8Function ExitMapEngine(V8ARGS){
    doneWithCurrentMap = true;
    nextMap = NULL;
    return v8::Undefined();
}

void ResetMapEngine(){
    nextMap = NULL;
    doneWithCurrentMap = false;
}

void TS_MapEngine(TS_Map *map){

    TS_MapEngineInstance *TS_MEGlobal = TS_GetGlobalMapEngineInstance();

    TS_MapEngineInstance MEInstance;
    MEInstance.camera = TS_Camera(0, 0);
    MEInstance.doneWithCurrentMap = false;
    MEInstance.currentMap = map;

    MEInstance.inputPerson = TS_MEGlobal->inputPerson;
    MEInstance.cameraPerson = TS_MEGlobal->cameraPerson;

    do{
        //currentMap->Calculate();
        nextMap = NULL;
        printf("Beginning Rendering loop.\n");
        while(!doneWithCurrentMap){
            MEInstance.currentMap->Update(TS_MEGlobal->camera);
            if(!GetGlobalMapData()->updateScript.IsEmpty()){
                GetGlobalMapData()->updateScript->Run();
            }
            MEInstance.currentMap->Render(TS_MEGlobal->camera);
            FlipScreenDL();
            //TODO: To make my computer not melt:
            //SDL_Delay(10);

            //currentMap->unoptDraw(camera);
            //SDL_GL_SwapBuffers();
        }

        TS_ClearTemporaryPersons();
        //delete currentMap;
        MEInstance.currentMap = TS_MEGlobal->nextMap;
    }while(nextMap!=NULL);

}

void TS_MapFinalizer(V8FINALIZERARGS) {
    TS_Map* map = (TS_Map*)parameter;
    delete map;
    object->Dispose();
}
