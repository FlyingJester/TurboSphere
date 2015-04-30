#ifndef ENGINE_HEAD
#define ENGINE_HEAD

#define V8FLAGS "--harmony_typed_arrays"

#include <string>
#include <cstring>
#include <cmath>

#include <jsapi.h>

#include <t5.h>
#include <opengame.h>

struct TS_ConfigOverride {
public:
	TS_ConfigOverride();
	~TS_ConfigOverride();
	bool gamefunc;
	bool sgmname;
    bool gamename;
    bool mainscript;
    bool screenwidth;
    bool screenheight;
    bool soundchannels;
    bool fullscreen;
    bool compositing;
    bool scale;
    bool systemfont;
    bool systemttffont;
    bool systemwindowstyle;
    bool systemarrow;
    bool systemuparrow;
    bool systemdownarrow;
    bool systemsoundfont;
    bool author;
	bool description;
	bool fixedplugins;
    bool plugins;
    TS_Config *config;
};

void runGame(const char * path, TS_ConfigOverride *overrideConf = NULL);

#endif
