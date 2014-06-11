#include "../libapi.h"
#include <GL/gl.h>
#include "../loadfunc/gl_extra.h"
#include "../config.h"
#include "image.h"
    GLfloat text[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    GLfloat fliptex[] = {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    GLfloat whiteft[] = {
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    };

TS_Image *CreateImage(int width, int height, RGBA* pixels){

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
    RGBA *newpixels = malloc(newWidth*newHeight<<2);


    for(int i = 0; i<height; ++i){
        //memcpy(newpixels+(i*width), pixels+(i*width), width<<2);

        //memcpy(newpixels+(i*(width+1))+1, pixels+(i*width), (width<<2));
        memcpy(newpixels+(i*(newWidth))+1, pixels+(i*width), (width<<2));
        //newpixels[((width+1)*i)] = 0xFFFFFFFF;
        newpixels[((newWidth)*i)] = 0xFFFFFFFF;
    }

    GLuint texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    if(configl.niceImages){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else{
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
 // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width+1, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, newpixels);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, newpixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    TS_Image *im = malloc(sizeof(TS_Image));
    GLfloat sx = 1.0f/(float)(newWidth);
    GLfloat ex = ((float)(width)/(float)(newWidth))+sx;
    GLfloat sy = 0.0f;//(float)(width);
    GLfloat ey = (float)(height)/(float)(newHeight);
    //zx = 1.0f;
    //GLfloat newtex[] = {zx, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, zx, 1.0f};
    GLfloat newtex[] = {sx, sy, ex, sy, ex, ey, sx, ey};
// im->pixels = NULL;//newpixels;
    glGenBuffers(1, &(im->TexCoordBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, im->TexCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<3, newtex, GL_STATIC_DRAW);
    if(true){
        glGenVertexArrays(1, &(im->VertexArray));
        glGenBuffers(1, &(im->VertexBuffer));
        glGenBuffers(1, &(im->ColorBuffer));
        glBindVertexArray(im->VertexArray);
        glVertexAttribPointer(GetTexCoordAttrib(), 2, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindBuffer(GL_ARRAY_BUFFER, im->ColorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<4, whitef, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(GetColorAttrib(), 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, im->VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<3, tex, GL_STREAM_DRAW); //Dummy value.
        glVertexAttribPointer(GetVertexAttrib(), 2, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindVertexArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    im->pixels = newpixels;
    im->texture = texture;
    im->w = width;
    im->h = height;

    return im;

}

TS_Image * CreateImageNerd(int width, int height, RGBA* pixels){

// RGBA *newpixels = malloc(width*height*4);

/*
for(int i = 0; i<height; ++i){
memcpy(newpixels+(i*width), pixels+(i*width), width*4);
}
*/
    GLuint texture = 0;
    TS_Image *im = malloc(sizeof(TS_Image));

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    if(configl.niceImages){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    else{
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    im->pixels = NULL;//newpixels;
    im->texture = texture;
    im->w = width;
    im->h = height;

    return im;

}

TS_Image * CloneImage(TS_Image * image){
    GLuint texture = 0;
    TS_Image *im = malloc(sizeof(TS_Image));
    glGenBuffers(1, &(im->TexCoordBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, im->TexCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<3, tex, GL_STATIC_DRAW);

    if(true){
        glGenVertexArrays(1, &(im->VertexArray));
        glGenBuffers(1, &(im->VertexBuffer));
        glGenBuffers(1, &(im->ColorBuffer));
        glBindVertexArray(im->VertexArray);
        glVertexAttribPointer(GetTexCoordAttrib(), 2, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindBuffer(GL_ARRAY_BUFFER, im->ColorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<4, whitef, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(GetColorAttrib(), 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, im->VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<3, tex, GL_STREAM_DRAW); //Dummy value.
        glVertexAttribPointer(GetVertexAttrib(), 2, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindVertexArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glGenerateMipmap(GL_TEXTURE_2D);
    if(configl.niceImages){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    else{
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glCopyImageSubData(image->texture, GL_TEXTURE_2D, 0, 0, 0, 0, texture, GL_TEXTURE_2D, 0, 0, 0, 0, image->w, image->h, 1);
    glGenerateMipmap(GL_TEXTURE_2D);

    im->pixels = NULL;
    im->texture = texture;
    im->w = image->w;
    im->h = image->h;

    return im;
}

TS_Image * GrabImage(int x, int y, int width, int height){

    GLuint texture = 0;
    TS_Image *im = malloc(sizeof(TS_Image));
    glGenBuffers(1, &(im->TexCoordBuffer));
    glBindBuffer(GL_ARRAY_BUFFER, im->TexCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<3, fliptex, GL_STATIC_DRAW);

    if(true){
        glGenVertexArrays(1, &(im->VertexArray));
        glGenBuffers(1, &(im->VertexBuffer));
        glGenBuffers(1, &(im->ColorBuffer));
        glBindVertexArray(im->VertexArray);
        glVertexAttribPointer(GetTexCoordAttrib(), 2, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindBuffer(GL_ARRAY_BUFFER, im->ColorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<4, whitef, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(GetColorAttrib(), 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, im->VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)<<3, fliptex, GL_STREAM_DRAW); //Dummy value.
        glVertexAttribPointer(GetVertexAttrib(), 2, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindVertexArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, ScreenCopy);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glGenerateMipmap(GL_TEXTURE_2D);
    if(configl.niceImages){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    else{
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ScreenCopy);
    glGenerateMipmap(GL_TEXTURE_2D);

    im->pixels = NULL;
    im->texture = texture;
    im->w = width;
    im->h = height;

    return im;

}

void DestroyImage(TS_Image* image){
    if(image->pixels)
        free(image->pixels);
    glDeleteTextures(1, &(image->texture));
    glDeleteBuffers(1, &(image->TexCoordBuffer));
    free(image);

}
