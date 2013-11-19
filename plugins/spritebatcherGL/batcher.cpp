#include "batcher.h"

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

TS_SpriteBatch::TS_SpriteBatch(){

    coords = std::vector<TS_SpriteTextureCoord>();
    glGenFramebuffers(1, &framebuffer);
    glGenTextures(1, &texture);

    //Set up the color buffer of framebuffer.
    glGenTextures(1, &fbotex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //Reserve the graphics memory for it.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH,  width);
    glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height);
    //Bind the color buffer to the framebuffer.

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbotex, 0);

    //glFrameBufferParameteri
    width = DEFAULT_WIDTH;
    height = DEFAULT_HEIGHT;

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

    coords.push_back({curwidth, curheight, (uint32_t)w, (uint32_t)h, 1.0f, 1.0f, 1.0f, 1.0f});

    glReadPixels(curwidth, curheight, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

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
