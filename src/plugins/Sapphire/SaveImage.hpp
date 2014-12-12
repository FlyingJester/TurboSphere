#pragma once

#include <array>
#include <map>
#include <SDL2/SDL.h>
#include <string>

#ifndef USE_TGA
  #define USE_TGA 1
#endif

#ifdef USE_TIFF
#undef USE_TIFF
#endif

namespace Sapphire {
namespace Save {

enum Formats {
  bmp,
#ifdef USE_TGA
  tga,
#endif
#ifdef USE_JPEG
  jpeg,
#endif
#ifdef USE_PNG
  png,
#endif
#ifdef USE_TIFF
  tiff,
#endif
#ifdef USE_GIF
  gif,
#endif
  num_formats
};

enum SaveStatus {ssSuccess, ssFailure};

// Preferred method:
SaveStatus Save(SDL_Surface *aToSave, const std::string &to);

// Other stuff, mostly internal.
typedef SaveStatus(*SaveFunction)(SDL_Surface *aToSave, const std::string &aPath);
typedef SaveStatus(*InitFunction)(void);

typedef std::pair<std::string, SaveFunction>SavePair;

SaveStatus InitFormats(void);
void InitSurfaceSave();

extern std::array<SaveFunction, num_formats> SaveFunctions;
extern std::array<InitFunction, num_formats> InitFunctions;
extern const std::map<std::string, SaveFunction> &SaveWithExtension;


}
}
