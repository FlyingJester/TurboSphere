#include "surface.h"
#include <SDL2/SDL_image.h>

void SurfaceInit(uint32_t ID) {
    IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF|IMG_INIT_WEBP);
    SurfaceObjInit(ID);
}

// Asynchronous
SDL_Surface *TS_CreateSurface(unsigned int w, unsigned int h, RGBA color) {
    SDL_Surface *surf = SDL_CreateRGBSurface(0, w, h, 32, CHANNEL_MASKS);
    SDL_SetSurfaceRLE(surf,1);
    surf->userdata = malloc(sizeof(surface_data));
    ((surface_data*)(surf->userdata))->mode = BLEND;

    SDL_FillRect(surf, NULL, color);
    return surf;
}
// Causes GL starvation
SDL_Surface *TS_GrabSurface(SDL_Rect *from);
// Surface Queued
int TS_SurfaceSetAlpha(SDL_Surface *surf, unsigned char alpha) {
    SDL_LockSurface(surf);

    for(int i = 0; i<surf->w*surf->h; i++) {
        ((unsigned char *)(surf->pixels))[(i<<2)+3] = alpha;
    }

    SDL_UnlockSurface(surf);
    return 1;
}
// Surface Queued, causes starvation
SDL_Surface *TS_SurfaceCloneSection(SDL_Surface *surf, SDL_Rect *from) {

    SDL_Surface *ret = SDL_CreateRGBSurface(0, from->w, from->h, 32, CHANNEL_MASKS);
    TS_PrimeSurface(ret);

    SDL_Rect to = {0, 0, from->w, from->h};
    SDL_BlitSurface(surf, from, ret, &to);

    return ret;

}
// Surface Queued
int TS_SurfaceSetBlendMode(SDL_Surface *surf, TS_Blendmode mode) {
    ((surface_data*)(surf->userdata))->mode = mode;

    return 1;

}
// Surface Queued
RGBA TS_SurfaceGetPixel(SDL_Surface *surf, unsigned int x, unsigned int y) {

    SDL_LockSurface(surf);

    unsigned int pxnum = (y*surf->w)+x;
    RGBA color = ((RGBA *)(surf->pixels))[pxnum];

    SDL_UnlockSurface(surf);

    return color;
}
// Surface Queued
int TS_SurfaceSetPixel(SDL_Surface *surf, unsigned int x, unsigned int y, RGBA color) {

    SDL_LockSurface(surf);

    unsigned int pxnum = (y*surf->w)+x;
    ((RGBA *)(surf->pixels))[pxnum] = color;

    SDL_UnlockSurface(surf);
    return 1;
}
// Surface Queued, causes surface starvation
SDL_Surface *TS_SurfaceRotate(SDL_Surface *surf, float angle, bool resize);
// Surface Queued
int TS_SurfaceFlipHorizontally(SDL_Surface *surf);
// Surface Queued
int TS_SurfaceFlipVertically(SDL_Surface *surf);
// Surface Queued, Entangles
int TS_SurfaceBlitMaskSurface(SDL_Surface *dest, SDL_Surface *surf, int x, int y, RGBA color);
// Surface Queued, Entangles
int TS_SurfaceTransformBlitMaskSurface(SDL_Surface *dest, SDL_Surface *surf, int x, int y, RGBA color);
// Surface Queued
int TS_SaveSurface(SDL_Surface *surf, const char *path);
