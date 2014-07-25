#include "CrossPlugin.hpp"
#include "api.hpp"
#include "script.hpp"
#include <cassert>
#include "SaveImage.hpp"
#include "Sapphire.hpp"
#include <memory>

class TS_FreeSurface{
public:
  void operator () (SDL_Surface *s) {SDL_FreeSurface(s);}
};

typedef Sapphire::GenericHolder<SDL_Surface, TS_FreeSurface> SurfaceHolder;

void TS_AddMethodToSurface(const char *aName, Turbo::JSCallback aFunc){

    assert(aFunc!=nullptr);
    assert(aName!=nullptr);
    assert((*aName)!='\0');

    Sapphire::Script::SurfaceJSObj.AddToProto(aName, aFunc);
}

int TS_SaveImage(const char *aPath, uint32_t *aPixels, unsigned aWidth, unsigned aHeight, bool absolute){

    assert(aPixels);

    const char * lEnding = aPath + strlen(aPath) - 1;
    while((*lEnding!='.') && (lEnding-1 != aPath))
      lEnding--;

    SDL_Surface *lSurf = SDL_CreateRGBSurfaceFrom(aPixels,aWidth,aHeight,IMAGE_DEPTH,aHeight << 2,CHANNEL_MASKS);

    std::unique_ptr<SurfaceHolder> lUPHolder(new SurfaceHolder(lSurf));

    std::map<std::string, Sapphire::Save::SaveFunction>::const_iterator lSaveFunc = Sapphire::Save::SaveWithExtension.find(std::string(lEnding));

    if(lSaveFunc!=Sapphire::Save::SaveWithExtension.end())
      return (*lSaveFunc).second(lSurf, aPath);

    SDL_SaveBMP(lSurf, aPath);
    return 1;

}
