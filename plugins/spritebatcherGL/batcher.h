#ifndef SPRITEBATCHER_BATCHER_HEAD
#define SPRITEBATCHER_BATCHER_HEAD

#include <vector>
#include "frontend.h"
#include "../common/graphic_common.h"

#define GL_GLEXT_PROTOTYPES 1
#include "../../SDL2/SDL_opengl.h"

//GL/glext.h, why you no do your job?
#include <GL/glext.h>

//This is an evil way to do things.
#define GL_FRAMEBUFFER_DEFAULT_WIDTH      0x9310
#define GL_FRAMEBUFFER_DEFAULT_HEIGHT     0x9311
#define GL_FRAMEBUFFER_DEFAULT_LAYERS     0x9312
#define GL_FRAMEBUFFER_DEFAULT_SAMPLES    0x9313

extern v8::Local<v8::Object> (*TS_SDL_GL_WrapTS_ColorDL)(TS_Color*);
extern void (APIENTRY * glFramebufferParameteri)(GLenum target, GLenum pname, GLint param);

EXTERN_OBJECT_TEMPLATES(SpriteBatchOp);
EXTERN_OBJECT_TEMPLATES(SpriteBatch);

typedef GLuint TS_Texture;

typedef struct TS_SpriteBatchOp {
    TS_Texture texture;     //Texture name.
    GLfloat TexCoords[8];   //Texture mapping coordinates
    GLuint Vertex[8];       //XY, Raster-Positional vertex locations
    GLuint Color[4];        //Per-Vertex 0xRRGGBBAA Color Mask
} TS_SpriteBatchOp;

TS_SpriteBatchOp *TS_CreateSpriteBatchOp(void);
void TS_FreeSpriteBatchOp(TS_SpriteBatchOp*);

void InitBatcher(void);

extern unsigned int DEFAULT_WIDTH;
extern unsigned int DEFAULT_HEIGHT;

v8Function SpriteBatchOpGetColor(V8GETTERARGS);
void SpriteBatchOpSetColor(V8SETTERARGS);

v8Function NewSpriteBatcher(V8ARGS);
v8Function NewSpriteBatchOp(V8ARGS);
v8Function spritebatcherAddImage(V8ARGS);
v8Function SpriteBatchGetImages(V8ARGS);

enum TS_SpriteBatchError {
    NOERROR = 0,
    NOROOM,     //The batch's spritesheet is too full.
    TOOBIG,     //The batch's spritesheet is not big enough in the first place.
    BADTEXTURE, //An invalid texture was passed.
};

enum TS_SpriteBatchTextureType {
    INVALID = 0,
    SDL_SURFACE,
    TS_IMAGE
};

class TS_SpriteTextureCoord{
public:
    int32_t  x;
    int32_t  y;
    uint32_t w;
    uint32_t h;
    float    x1ord;
    float    y1ord;
    float    x2ord;
    float    y2ord;
};

class TS_SpriteBatch {
public:
    friend v8Function SpriteBatchGetImages(V8ARGS);

    TS_SpriteBatch();
    ~TS_SpriteBatch();

    TS_SpriteBatchError AddImage(int w, int h, void *pixels, bool RGBA = true);
    TS_SpriteBatchError AddImage(int w, int h, TS_Texture tex);
    int width, height, curwidth, curheight, rowheight;

    void *getPixels();
    size_t numImages;
    std::vector<TS_SpriteTextureCoord> coords;
    TS_Texture texture;
    GLuint framebuffer;

    std::vector<TS_SpriteBatchOp*> operations;

    TS_SpriteBatchError PushOperation(TS_SpriteBatchOp *op);

private:
    inline void SetOrtho(void);
    TS_Texture fbotex;
    float heightf;
    float widthf;

};


#endif  //SPRITEBATCHER_BATCHER_HEAD
