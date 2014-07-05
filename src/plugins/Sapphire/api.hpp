#pragma once

#include <SDL2/SDL.h>
#include "Image.hpp"
#include <color.h>

/////
// This file contains all Script-facing functions. The JS-Wrapping unctions
// are contained in script.hpp.
//

namespace Sapphire {

//TODO: Commented out functions are still unimplemented.

SDL_Surface *LoadSurface(const char *aPath);
SDL_Surface *CreateSurface(unsigned aWidth, unsigned aHeight, TS_Color *color);
SDL_Surface *FromImage(Image *aFrom);

// Galileo Functions require the Galileo namespace to be restored.



}
