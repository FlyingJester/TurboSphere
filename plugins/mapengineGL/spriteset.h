#ifndef MAPENGINE_SS_HEAD
#define MAPENGINE_SS_HEAD

#include "typedef.h"

#include <vector>

#include "mapengine.h"

EXTERN_OBJECT_TEMPLATES(SpriteSet);

void InitSpriteSet(void);
void CloseSpriteSet(void);
void TS_SpriteSetFinalizer(V8FINALIZERARGS);

v8Function LoadSpriteSet(V8ARGS);


#ifdef MSC_VER

    #define MEMALIGN(X) _declspec(align(X))

#else

    #define MEMALIGN(X) __attribute__((aligned(X)))
#endif

struct TS_SpriteSetFrame{
    int imageIndex;
    int delay;
};

struct TS_SpriteSetDirection{
    char *name;
    int numFrames;
    TS_SpriteSetFrame *frames;
};

class MEMALIGN(4) TS_SpriteSet{
public:
    TS_SpriteSet(const char *filename);
    ~TS_SpriteSet();

    TS_Texture *textures;
    int numTextures;

    int textureWidth;
    int textureHeight;

    SDL_Rect base;
    int numDirections;
    TS_SpriteSetDirection *directions;
};

#endif
