#include "image.h"
using namespace Sapphire;


void FreePixels(RGBA *pix){
    delete[] pix;
}

RGBA *AllocPixels(size_t w, size_t h){
    return new RGBA[(w*h)<<2];
}
/*
/////
// As prototyped in FJ-GL

class image{
public:

    image();
    ~image();

    /////
    // Do not call a non-trivial constructor from any thread but the main thread.
    image(image *);
    image(const image &);
    image(int width, int height, RGBA *pixels);
    image(SDL_Surface *);
    image(int x, int y, int width, int height);

    void Blit(int x, int y, BlendMode blendmode));
    void BlitMask(IMAGE * image, int x, int y, BlendMode blendmode, RGBA mask, BlendMode mask_blendmode));
    void TransformBlit(int x[4], int y[4], BlendMode blendmode));
    void TransformBlitMask(int x[4], int y[4], BlendMode blendmode, RGBA mask, BlendMode mask_blendmode));
    RGBA* Lock(void);
    void Unlock(void);

    RGBA *pixels;
    GLuint texture;
    unsigned int w;
    unsigned int h;
    GLuint TexCoordBuffer;
    GLuint ColorBuffer;
    GLuint VertexBuffer;
    GLuint VertexArray;
};
*/

Sapphire::image::image(){
    freePixels = FreePixels;
    allocPixels = AllocPixels;

    pixels = NULL;
    texture         = 0;
    TexCoordBuffer  = 0;
    ColorBuffer     = 0;
    VertexBuffer    = 0;
    VertexArray     = 0;

    w = 0;
    h = 0;
}

Sapphire::image::~image(){
    if(pixels != NULL)
        freePixels(pixels);

    if(texture)
        glDeleteTextures(1, &texture);

    if(TexCoordBuffer)
        glDeleteBuffers(1, &TexCoordBuffer);

    if(ColorBuffer)
        glDeleteBuffers(1, &ColorBuffer);

    if(VertexBuffer)
        glDeleteBuffers(1, &VertexBuffer);
    if(VertexArray)
        glDeleteVertexArrays(1, &VertexArray);

}

Sapphire::image::image(const image &){

}

Sapphire::image::image(int width, int height, RGBA *pixels){

    int newWidth = width+1;
    int newHeight = height;
/////
// The value 32 as a whole factor for dimensions is taken from Apple's documentation, specifically for Intel cards.

/////
// Ensure the image is at least 32x32

    while(newHeight%32)
        newHeight++;

    while(newWidth%32)
        newWidth++;

    int nw = 32;
    int nh = 32;

/////
// Size up to the next power of two.
// Dimensions will remain divisble by 32 because of math.

    while(nw<newWidth)
        nw<<=1;
    while(nh<newHeight)
        nh<<=1;

    newWidth = nw;
    newHeight = nh;

    //RGBA *newpixels = malloc((width+1)*height*4);
    RGBA *newpixels = allocPixels(newWidth, newHeight);


    for(int i = 0; i<height; ++i){
        memcpy(newpixels+(i*(newWidth))+1, pixels+(i*width), (width<<2));

        newpixels[((newWidth)*i)] = 0xFFFFFFFF;
    }

    GLuint texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, newpixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    GLfloat sx = 1.0f/(float)(newWidth);
    GLfloat ex = ((float)(width)/(float)(newWidth))+sx;
    GLfloat sy = 0.0f;//(float)(width);
    GLfloat ey = (float)(height)/(float)(newHeight);
    //zx = 1.0f;
    GLfloat newtex[] = {sx, sy, ex, sy, ex, ey, sx, ey};
    glGenBuffers(1, &(TexCoordBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, TexCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<3, newtex, GL_STATIC_DRAW);

    /////
    // Vertex Array Setup

        glGenVertexArrays(1, &(VertexArray));
        glGenBuffers(1, &(VertexBuffer));
        glGenBuffers(1, &(ColorBuffer));
        glBindVertexArray(VertexArray);
        glVertexAttribPointer(GetTexCoordAttrib(), 2, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<4, whitef, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(GetColorAttrib(), 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<3, tex, GL_STREAM_DRAW); //Dummy value.
        glVertexAttribPointer(GetVertexAttrib(), 2, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindVertexArray(0);
    // End Vertex Array setup

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    pixels = newpixels;
    texture = texture;
    w = width;
    h = height;
}

/*

Sapphire::image * CreateImageNerd(int width, int height, RGBA* pixels){

// RGBA *newpixels = malloc(width*height*4);
*/
/*
for(int i = 0; i<height; ++i){
memcpy(newpixels+(i*width), pixels+(i*width), width*4);
}
*/
/*
    GLuint texture = 0;
    IMAGE *im = malloc(sizeof(IMAGE));

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    pixels = NULL;//newpixels;
    texture = texture;
    w = width;
    h = height;

    return im;

}
*/
Sapphire::image::image(Sapphire::image *image){
    GLuint texture = 0;
    glGenBuffers(1, &(TexCoordBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, TexCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<3, tex, GL_STATIC_DRAW);

    /////
    // Vertex Array Setup
    glGenVertexArrays(1, &(VertexArray));
    glGenBuffers(1, &(VertexBuffer));
    glGenBuffers(1, &(ColorBuffer));
    glBindVertexArray(VertexArray);
    glVertexAttribPointer(GetTexCoordAttrib(), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<4, whitef, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(GetColorAttrib(), 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<3, tex, GL_STREAM_DRAW); //Dummy value.
    glVertexAttribPointer(GetVertexAttrib(), 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindVertexArray(0);
    // end Vertex Array Setup

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glCopyImageSubData(image->texture, GL_TEXTURE_2D, 0, 0, 0, 0, texture, GL_TEXTURE_2D, 0, 0, 0, 0, image->w, image->h, 1);
    glGenerateMipmap(GL_TEXTURE_2D);

    pixels = NULL;
    texture = texture;
    w = image->w;
    h = image->h;

}
/*
Sapphire::image::image(int x, int y, int width, int height);

void  Sapphire::image::Blit(int x, int y, BlendMode blendmode));
void  Sapphire::image::BlitMask(IMAGE * image, int x, int y, BlendMode blendmode, RGBA mask, BlendMode mask_blendmode));
void  Sapphire::image::TransformBlit(int x[4], int y[4], BlendMode blendmode));
void  Sapphire::image::TransformBlitMask(int x[4], int y[4], BlendMode blendmode, RGBA mask, BlendMode mask_blendmode));
*/
RGBA* Sapphire::image::Lock(void){
    return pixels;
}

void  Sapphire::image::Unlock(void){

}
