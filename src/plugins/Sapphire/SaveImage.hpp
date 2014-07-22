#pragma once

#include <array>
#include <SDL2/SDL.h>
#include <string>

namespace Sapphire {
namespace Save {

enum Formats {
  bmp,
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

typedef SaveStatus(*SaveFunction)(SDL_Surface *aToSave, const std::string &aPath);
typedef SaveStatus(*InitFunction)(void);

SaveStatus InitFormats(void);
void InitSurfaceSave();

extern std::array<SaveFunction, num_formats> SaveFunctions;
extern std::array<InitFunction, num_formats> InitFunctions;


}
}
