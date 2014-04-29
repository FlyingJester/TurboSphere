#ifndef SAPPHIRE_SURFACE_HEAD
#define SAPPHIRE_SURFACE_HEAD

#include <stdbool.h>
#include <stdint.h>
#include "fjgl.h"
#include "blendmodes.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>

#define Frmask 0x000000ff
#define Fgmask 0x0000ff00
#define Fbmask 0x00ff0000
#define Famask 0xff000000

#define CHANNEL_MASKS Frmask, Fgmask, Fbmask, Famask

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct{
        TS_Blendmode mode;
    } surface_data;

    void SurfaceInit(uint32_t ID);

    inline void TS_PrimeSurface(SDL_Surface *surf){
        SDL_SetSurfaceRLE(surf,1);
        surf->userdata = malloc(sizeof(surface_data));
        ((surface_data*)(surf->userdata))->mode = BLEND;
    }

    inline void TS_ClearSurface(SDL_Surface *surf){
        free(surf->userdata);
    }

    // Asynchronous
    SDL_Surface *TS_CreateSurface(unsigned int w, unsigned int h, RGBA color);
    // Causes GL starvation
    SDL_Surface *TS_GrabSurface(SDL_Rect *from);
    // Surface Queued
    int TS_SurfaceSetAlpha(SDL_Surface *surf, unsigned char alpha);
    // Surface Queued, causes starvation
    SDL_Surface *TS_SurfaceCloneSection(SDL_Surface *surf, SDL_Rect *from);
    // Surface Queued
    int TS_SurfaceSetBlendMode(SDL_Surface *surf, TS_Blendmode mode);
    // Surface Queued
    RGBA TS_SurfaceGetPixel(SDL_Surface *surf, unsigned int x, unsigned int y);
    // Surface Queued
    int TS_SurfaceSetPixel(SDL_Surface *surf, unsigned int x, unsigned int y, RGBA color);
    // Surface Queued
    int TS_SurfaceGradientLine(SDL_Surface *surf, int x[2], int y[2], RGBA color[2]);
    // Surface Queued
    int TS_SurfaceGradientTriangle(SDL_Surface *surf, int x[3], int y[3], RGBA color[3]);
    // Surface Queued
    int TS_SurfaceOutlinedEllipse(SDL_Surface *surf, int x, int y, unsigned int radius, RGBA color);
    // Surface Queued
    int TS_SurfaceGradientEllipse(SDL_Surface *surf, int x, int y, unsigned int radius, RGBA color[2]);
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

#ifdef __cplusplus
}
#endif

#endif
