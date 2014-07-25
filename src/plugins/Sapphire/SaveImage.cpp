#include "SaveImage.hpp"
#include <cassert>
#include "Sapphire.hpp"

class FileHolder {
    FILE *mFile;
    public:
    FileHolder(FILE *aFile)
      : mFile(aFile){}

    ~FileHolder(){fflush(mFile); fclose(mFile);}
};

class SurfaceHolder {
    SDL_Surface *mSurface;
    public:
    SurfaceHolder(SDL_Surface *aSuface)
      : mSurface(aSuface){
        SDL_LockSurface(aSuface);
      }

    ~SurfaceHolder(){ SDL_UnlockSurface(mSurface);}
};

template<typename T>
class ArrayHolder {
    T *mT;
    public:
    ArrayHolder(T *aT)
      : mT(aT){}

    ~ArrayHolder(){delete[] mT;}
};

#ifdef USE_JPEG

#endif
#ifdef USE_PNG

  #include "Formats/SavePNG.cpp"

#endif
#ifdef USE_TIFF



#endif
#ifdef USE_GIF



#endif
#ifdef USE_TGA

  #include "Formats/SaveTGA.cpp"

#endif

namespace Sapphire {
namespace Save {

std::array<SaveFunction, num_formats> SaveFunctions = std::array<SaveFunction, num_formats> ();
std::array<InitFunction, num_formats> InitFunctions = std::array<InitFunction, num_formats> ();
std::map<std::string, SaveFunction> SaveViaExtension = std::map<std::string, SaveFunction> ();
const std::map<std::string, SaveFunction> &SaveWithExtension = SaveViaExtension;


SaveStatus BMPSaveFunction(SDL_Surface *aToSave, const std::string &aPath){

  assert(aToSave);
  assert(!aPath.empty());

  int err = SDL_SaveBMP(aToSave, aPath.c_str());

  if(err==0)
    return ssSuccess;

  return ssFailure;

}

SaveStatus BMPInitFunction(void){
  SaveFunctions[Formats::bmp] = BMPSaveFunction;
  return ssSuccess;
}

SaveStatus InitFormats(void){

  InitFunctions[Formats::bmp] = BMPInitFunction;
  SaveViaExtension["bmp"]     = BMPSaveFunction;

#ifdef USE_JPEG
  InitFunctions[Formats::jpeg] = JPEGInitFunction;
  if(SaveFunctions[Formats::jpeg]){
    SaveViaExtension["jpeg"]    = SaveFunctions[Formats::jpeg];
    SaveViaExtension["jpg"]     = SaveFunctions[Formats::jpeg];
  }
#endif
#ifdef USE_PNG
  InitFunctions[Formats::png] = PNGInitFunction;
  if(SaveFunctions[Formats::png])
    SaveViaExtension["png"]     = SaveFunctions[Formats::png];
#endif
#ifdef USE_TIFF
  InitFunctions[Formats::tiff] = TIFFInitFunction;
  if(SaveFunctions[Formats::tiff])
    SaveViaExtension["tiff"]    = SaveFunctions[Formats::tiff];
#endif
#ifdef USE_GIF
  InitFunctions[Formats::gif] = GIFInitFunction;
  if(SaveFunctions[Formats::gif])
    SaveViaExtension["gif"]     = SaveFunctions[Formats::gif];
#endif
#ifdef USE_TGA
  InitFunctions[Formats::tga] = TGAInitFunction;
  if(SaveFunctions[Formats::tga])
    SaveViaExtension["tga"]     = SaveFunctions[Formats::tga];
#endif

  return ssSuccess;

}

void InitSurfaceSave(){
  InitFormats();
  auto InitIter = InitFunctions.begin();
  while(InitIter!=InitFunctions.end()){
    (*InitIter)();
    InitIter++;
  }
  InitFormats();
}

}
}
