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

void TS_AddMethodToSurface(const char *aName, JSnative aFunc){

    assert(aFunc!=nullptr);
    assert(aName!=nullptr);
    assert((*aName)!='\0');

    Sapphire::Script::memberiter_t lIter = Sapphire::Script::CrossPluginSurfaceMembers.begin();

    while(lIter!=Sapphire::Script::CrossPluginSurfaceMembers.end()){
        if(strcmp((*lIter).first, aName))
          continue;

        printf(BRACKNAME " TS_AddMethodToSurface Warning: Overwriting cross-plugin Surface member %s.\n", aName);
        (*lIter).second = aFunc;
        return;
    }

    Sapphire::Script::CrossPluginSurfaceMembers.push_back(Sapphire::Script::CallbackWithName(aName, aFunc));
}

int TS_SaveImage(const char *aPath, uint32_t *aPixels, unsigned aWidth, unsigned aHeight, bool absolute){

    assert(aPixels);

    SDL_Surface *lSurf = SDL_CreateRGBSurfaceFrom(aPixels,aWidth,aHeight,IMAGE_DEPTH,aHeight << 2,CHANNEL_MASKS);

    Sapphire::Save::Save(lSurf, aPath);

    SDL_FreeSurface(lSurf);

    return 1;

}
