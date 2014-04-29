#ifndef SAPPHIRE_IMAGE_HEAD
#define SAPPHIRE_IMAGE_HEAD

#include <SDL2/SDL_image.h>

#define IMG_FLAGS (IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF)

#include "../backend.h"

#ifdef __cplusplus
#include <cstdlib>

void InitImage(uint32_t ID);
void CloseImage(void);

#endif

#ifdef __cplusplus
struct TS_Image{
#else
typedef struct {
#endif
    RGBA *pixels;
    GLuint texture;
    unsigned int w;
    unsigned int h;
GLuint TexCoordBuffer;
GLuint ColorBuffer;
GLuint VertexBuffer;
GLuint VertexArray; //Only valid when configl.hasVertexArrays is not zero.
#ifdef __cplusplus
};
#else
} TS_Image;
#endif



#endif
