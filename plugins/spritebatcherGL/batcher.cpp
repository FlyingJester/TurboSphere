#include "batcher.h"
#include <cstdlib>
/*
class TS_SpriteBatch {
public:

    TS_SpriteBatch();
    ~TS_SpriteBatch();

    TS_SpriteBatchError AddImage(int w, int h, void *pixels, bool RGBA = true);

    int w, h, curwidth, curheight, rowheight;

    std::vector<TS_SpriteTextureCoord> coords;
    TS_Texture texture;

    void blit(int x, int y);



};
*/
#include <cstdio>


void InitBatcher(void){
    printf("Limit on size are %i.\n", GL_MAX_TEXTURE_SIZE);
    TS_SpriteBatch *batch = new TS_SpriteBatch();

    uint32_t pixels[] = {
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,

    0xFF00FFFF,
    0xFF00FFFF,
    0xFF00FFFF,
    0xFF00FFFF,

    0xFF0000FF,
    0xFF0000FF,
    0xFF0000FF,
    0xFF0000FF,

    0x0000FFFF,
    0x0000FFFF,
    0x0000FFFF,
    0x0000FFFF
    };

    batch->AddImage(4, 4, pixels);

}

TS_SpriteBatch::TS_SpriteBatch(){

    printf("Error codes are:\n\t%i\tGL_INVALID_ENUM\n\t%i\tGL_INVALID_VALUE\n\t%i\tGL_INVALID_OPERATION\n\t%i\tGL_INVALID_FRAMEBUFFER_OPERATION\n\t%i\tGL_OUT_OF_MEMORY\n\t%i\tGL_STACK_UNDERFLOW\n\t%i\tGL_STACK_OVERFLOW\n\n",
                                    GL_INVALID_ENUM,       GL_INVALID_VALUE,       GL_INVALID_OPERATION,       GL_INVALID_FRAMEBUFFER_OPERATION,       GL_OUT_OF_MEMORY,       GL_STACK_UNDERFLOW,       GL_STACK_OVERFLOW);

    //glFrameBufferParameteri
    width = DEFAULT_WIDTH;
    height = DEFAULT_HEIGHT;
    widthf = DEFAULT_WIDTH;
    heightf = DEFAULT_HEIGHT;

    const void *pixels = calloc(DEFAULT_WIDTH*DEFAULT_HEIGHT, 4);
    coords = std::vector<TS_SpriteTextureCoord>();
    glGenFramebuffers(1, &framebuffer);
    glGenTextures(1, &texture);
    GLenum err = glGetError();

    if(err)
        printf("1. We had an error %i.\n", err);


    //Set up the color buffer of framebuffer.
    glGenTextures(1, &fbotex);
    err = glGetError();

    if(err)
        printf("2. We had an error %i.\n", err);

    glBindTexture(GL_TEXTURE_2D, fbotex);
    err = glGetError();

    if(err)
        printf("3. We had an error %i.\n", err);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    err = glGetError();

    if(err)
        printf("4. We had an error %i.\n", err);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    err = glGetError();

    if(err)
        printf("5. We had an error %i.\n", err);
    //Reserve the graphics memory for it.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    err = glGetError();

    if(err)
        printf("6. We had an error %i.\n", err);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    err = glGetError();

    if(err)
        printf("7. We had an error %i.\n", err);
    glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH,  width);
        err = glGetError();

    if(err)
        printf("8. We had an error %i.\n", err);
    glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height);
    err = glGetError();

    if(err)
        printf("9. We had an error %i.\n", err);
    //Bind the color buffer to the framebuffer.

    err = glGetError();

    if(err)
        printf("A. We had an error %i.\n", err);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbotex, 0);

    err = glGetError();

    if(err)
        printf("B. We had an error %i.\n", err);

}

TS_SpriteBatch::~TS_SpriteBatch(){
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &texture);
}

TS_SpriteBatchError TS_SpriteBatch::AddImage(int w, int h, void *pixels, bool RGBA){

    if((w>width)||(h>height)){
        return TS_SpriteBatchError::TOOBIG;
    }

    if(curwidth+w>width){
        curheight+=rowheight;
        rowheight = 0;
        curwidth = 0;
    }

    if(curheight+h>height){
        return TS_SpriteBatchError::NOROOM;
    }

    //TODO: The floats are dummy values.
    coords.push_back({curwidth, curheight, (uint32_t)w, (uint32_t)h, ((float)curwidth)/((float)width), ((float)curheight)/((float)height), ((float)(curwidth+w))/((float)width), ((float)(curheight+w))/((float)height)});

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

    TS_Texture tex;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    const GLint   vertexData[]   = {curwidth, curheight, curwidth+w, curheight, curwidth+w, curheight+h, curwidth, curheight+h};
    const GLint   texcoordData[] = {0, 0, 1, 0, 1, 1, 0, 1};
    const GLuint  colorData[]    = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

    glTexCoordPointer(2, GL_INT, 0, texcoordData);
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    curwidth+=w;

    if(rowheight<h)
        rowheight=h;

    return TS_SpriteBatchError::NOERROR;
}

/*
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
*/


TS_SpriteBatchError TS_SpriteBatch::AddImage(int w, int h, TS_Texture tex){

//  coords.push_back(TS_SpriteTextureCoord());
    coords.push_back({curwidth, curheight, (uint32_t)w, (uint32_t)h, 1.0f, 1.0f, 1.0f, 1.0f});
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);



    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    return TS_SpriteBatchError::NOERROR;
}

void *TS_SpriteBatch::getPixels(){
    void *pixels = malloc(4*width*height);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    return pixels;
}
