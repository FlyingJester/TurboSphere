#include "api.hpp"
#include "Sapphire.hpp"
#include "Image.hpp"
#include <opengame.h>

namespace Sapphire {

static TS_Directories *TS_Dirs = GetDirs();

// TODO: new T5!
SDL_Surface *LoadSurface(const char *aPath, bool aAbsolute){

    const char * lPath = aPath;
    char *lsPath = nullptr;

    if(!aAbsolute){
        const size_t aPathLen  = strlen(aPath);
        const size_t aImageLen = strlen(TS_Dirs->image);
        lsPath = (char *)alloca(aPathLen + aImageLen + 1);
        lsPath[aPathLen+aImageLen] = 0;
        memcpy(lsPath, TS_Dirs->image, aImageLen);
        memcpy(lsPath+aImageLen, aPath, aPathLen);
        lPath = lsPath;
    }

    SDL_Surface *lAnyFormatSurface = IMG_Load(aPath);

    if(lAnyFormatSurface==nullptr){
        fprintf(stderr, P_ERROR "Could not load Image %s. %s\n", aPath, SDL_GetError());
        return nullptr;
    }

    SDL_Rect lRect = {0, 0, lAnyFormatSurface->w, lAnyFormatSurface->h};

    SDL_Surface* rCorrectFormatSurface = GenerateSurface(lRect);// SDL_CreateRGBSurface(0, lRect.w, lRect.h, IMAGE_DEPTH, CHANNEL_MASKS);
    assert(rCorrectFormatSurface);

    SDL_LowerBlit(lAnyFormatSurface, &lRect, rCorrectFormatSurface, &lRect);

    SDL_FreeSurface(lAnyFormatSurface);

    return rCorrectFormatSurface;
}

SDL_Surface *CreateSurface(unsigned aWidth, unsigned aHeight, TS_Color *color){
    const uint32_t Color = color->toInt();

    SDL_Surface* rSurface =  GenerateSurface(aWidth, aHeight);//SDL_CreateRGBSurface(0, aWidth, aHeight, IMAGE_DEPTH, CHANNEL_MASKS);
    assert(rSurface);

    SDL_LockSurface(   rSurface );
    {
        uint32_t *lPixels = (uint32_t *)(rSurface->pixels);
        std::fill(lPixels, &(lPixels[aWidth*aHeight]), Color);
    }
    SDL_UnlockSurface( rSurface );

    return rSurface;
}

SDL_Surface *FromImage(Image *aFrom){
    SDL_Surface* rSurface =  GenerateSurface(aFrom->Width(), aFrom->Height());

    SDL_LockSurface(   rSurface );
    {
        aFrom->CopyData(rSurface->pixels);
    }
    SDL_UnlockSurface( rSurface );

    return rSurface;

}

}
