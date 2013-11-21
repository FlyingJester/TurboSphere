#ifndef SPRITEBATCHER_BATCHER_HEAD
#define SPRITEBATCHER_BATCHER_HEAD

#include <vector>

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

void InitBatcher(void);

#define DEFAULT_WIDTH  512
#define DEFAULT_HEIGHT 512

typedef GLuint TS_Texture;

enum TS_SpriteBatchError {NOERROR = 0,
    NOROOM, //The batch's spritesheet is too full.
    TOOBIG  //The batch's spritesheet is not big enough in the first place.
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

    TS_SpriteBatch();
    ~TS_SpriteBatch();

    TS_SpriteBatchError AddImage(int w, int h, void *pixels, bool RGBA = true);
    TS_SpriteBatchError AddImage(int w, int h, TS_Texture tex);

    int width, height, curwidth, curheight, rowheight;

    void *getPixels();

    std::vector<TS_SpriteTextureCoord> coords;
    TS_Texture texture;
    GLuint framebuffer;

    void blit(int x, int y);

private:
    TS_Texture fbotex;
    float heightf;
    float widthf;

};


#endif  //SPRITEBATCHER_BATCHER_HEAD
