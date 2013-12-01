#ifndef SPRITEBATCHER_BATCHER_HEAD
#define SPRITEBATCHER_BATCHER_HEAD

#include <vector>
#include "frontend.h"

#define GL_GLEXT_PROTOTYPES 1
#include "../../SDL2/SDL_opengl.h"

//GL/glext.h, why you no do your job?
#include <GL/glext.h>

//This is an evil way to do things.
#define GL_FRAMEBUFFER_DEFAULT_WIDTH      0x9310
#define GL_FRAMEBUFFER_DEFAULT_HEIGHT     0x9311
#define GL_FRAMEBUFFER_DEFAULT_LAYERS     0x9312
#define GL_FRAMEBUFFER_DEFAULT_SAMPLES    0x9313

extern void (APIENTRY * glFramebufferParameteri)(GLenum target, GLenum pname, GLint param);

EXTERN_OBJECT_TEMPLATES(SpriteBatch);

void InitBatcher(void);

extern unsigned int DEFAULT_WIDTH;
extern unsigned int DEFAULT_HEIGHT;

v8Function NewSpriteBatcher(V8ARGS);
v8Function spritebatcherDebug(V8ARGS);
v8Function spritebatcherAddImage(V8ARGS);
v8Function spritebatcherBlitBuffer(V8ARGS);
v8Function SpriteBatchGetImages(V8ARGS);

typedef GLuint TS_Texture;

enum TS_SpriteBatchError {
    NOERROR = 0,
    NOROOM, //The batch's spritesheet is too full.
    TOOBIG  //The batch's spritesheet is not big enough in the first place.
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

class TS_BatchOperation{
public:
    //virtual void blit(void) = 0;
    //virtual void Destroy(void) = 0;
    virtual void Set(int x, int y, float *coords, TS_Texture tex, uint32_t *color, int32_t *vertices) = 0;

    TS_Texture tex;

    size_t coordsSize;
    float *coords;

    int32_t x, y;

    size_t colorSize;
    uint32_t *color;

    size_t vertexSize;
    int32_t *vertices;

};

class TS_BatchImageOperation : public TS_BatchOperation{
public:
    TS_BatchImageOperation();
    ~TS_BatchImageOperation();

    virtual void Set(int x, int y, float coords[8], TS_Texture tex, uint32_t color[4], int32_t vertices[8]);
    //virtual void blit(void);
    //virtual void Destroy(void);
/*
    TS_Texture tex;

    size_t coordsSize;
    float coords[8];

    int32_t x, y;

    size_t colorSize;
    uint32_t color[4];
*/
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

    void blit(int x, int y);
    void blitDebug(int x, int y);

    std::vector<TS_BatchOperation> operations;

private:
    inline void SetOrtho(void);
    TS_Texture fbotex;
    float heightf;
    float widthf;

};


#endif  //SPRITEBATCHER_BATCHER_HEAD
