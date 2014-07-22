#pragma once

#include <SDL2/SDL.h>
#include "Sapphire.hpp"
#include "Image.hpp"
#include <color.h>

/////
// This file contains functions that exist solely to simplify Script-facing functions.
// The JS-Wrapping unctions are contained in script.hpp/script.cpp.
//

namespace Sapphire {

//TODO: Commented out functions are still unimplemented.

SDL_Surface *LoadSurface(const char *aPath, bool aAbsolute = false);
SDL_Surface *CreateSurface(unsigned aWidth, unsigned aHeight, TS_Color *color);
SDL_Surface *FromImage(Image *aFrom);


inline SDL_Surface *GenerateSurface(unsigned aWidth, unsigned aHeight){
    return SDL_CreateRGBSurface(0, aWidth, aHeight, IMAGE_DEPTH, CHANNEL_MASKS);
}

template<typename T>
inline SDL_Surface *GenerateSurface(T a){
    return SDL_CreateRGBSurface(0, a.w, a.h, IMAGE_DEPTH, CHANNEL_MASKS);
}

// Galileo Functions require the Galileo namespace to be restored.



}
