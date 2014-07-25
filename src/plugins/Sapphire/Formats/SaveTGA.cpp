#ifndef USE_TGA
#error
#endif

namespace Sapphire{
namespace Save{

SaveStatus TGASaveFunction(SDL_Surface *aToSave, const std::string &aPath);

SaveStatus TGAInitFunction(void){
  SaveFunctions[Formats::tga] = TGASaveFunction;
  return ssSuccess;
}

namespace TGA{

  const unsigned RLE    = 0x08;
  const unsigned RGBA   = 0x02;
  const unsigned NO_MAP = 0x00;
  const unsigned IDSize = 28;
  const uint8_t TurboSphereID[IDSize] = {
    'T', 'u', 'r', 'b', 'o', 'S', 'p', 'h', 'e', 'r', 'e', 0xE2, 0x80, 0x82,
    'S', 'a', 'p', 'p', 'h', 'i', 'r', 'e', 0xE2, 0x80, 0x82, 'T', 'G', 'A',
  };

  const unsigned BasicHeaderSize = 12;
  const uint8_t BasicHeaderRLE[BasicHeaderSize] = {
    IDSize, NO_MAP, RLE|RGBA,
    0, 0, 0, 0, 0, // Colormap: char origin [2], char length [2], char depth[1]
    0, 0, 0, 0, // X and Y origins as shorts.
  };
  const uint8_t BasicHeaderRAW[BasicHeaderSize] = {
    IDSize, NO_MAP, RGBA,
    0, 0, 0, 0, 0, // Colormap: char origin [2], char length [2], char depth[1]
    0, 0, 0, 0, // X and Y origins as shorts.
  };

  const uint32_t rmask = 0x00ff0000;
  const uint32_t gmask = 0x0000ff00;
  const uint32_t bmask = 0x000000ff;
  const uint32_t amask = 0xff000000;

  #define TGA_CHANNEL_MASKS TGA::rmask, TGA::gmask, TGA::bmask, TGA::amask

}
int PixelMatches(unsigned aRunSoFar, unsigned aMaxRun, uint32_t sig, uint32_t * &val, uint32_t *maxval);

int WriteMatches(FILE *aTo, uint32_t *aFrom, uint32_t *aMax){
  while(aFrom!=aMax){
    uint32_t lSig = *aFrom;
    int run = PixelMatches(0, 0x7F, lSig, aFrom, aMax);
    fputc((run-1)|0x80, aTo);
    fwrite(&lSig, 1, 4, aTo);
  }
  return ssSuccess;
}

int WriteMatchesV(FILE *aTo, void *aFrom, void *aMax){
  return WriteMatches(aTo, static_cast<uint32_t *>(aFrom),
                            static_cast<uint32_t *>(aMax));
}

int PixelMatches(unsigned aRunSoFar, unsigned aMaxRun, uint32_t sig, uint32_t * &val, uint32_t *maxval){
  if( (val==maxval ) || ( aRunSoFar == aMaxRun ) )
    return aRunSoFar;

  if(val[0]==sig)
    return PixelMatches(++aRunSoFar, aMaxRun, sig, ++val, maxval);

  return aRunSoFar;
}

SaveStatus TGASaveFunction(SDL_Surface *aToSave, const std::string &aPath){

  assert(aToSave);
  assert(!aPath.empty());

  FILE *lFile = fopen(aPath.c_str(), "wb");
  if(lFile==nullptr)
    return ssFailure;

  unsigned short lDimensions[2] = {std::min<unsigned short>(aToSave->w, 0xFFFF), std::min<unsigned short>(aToSave->h, 0xFFFF)};

  fwrite(TGA::BasicHeaderRLE, 1, TGA::BasicHeaderSize,    lFile);
  fwrite(lDimensions,    sizeof(short),      2, lFile);
  fputc(IMAGE_DEPTH, lFile);

  // The secret Sauce:
  fputc((0x20|0x09), lFile);

  SDL_Surface *lTGAFormatSurface = SDL_CreateRGBSurface(0, aToSave->w, aToSave->h, IMAGE_DEPTH, TGA_CHANNEL_MASKS);

  SDL_Rect lRect = {0, 0, aToSave->w, aToSave->h};
  SDL_LowerBlit(aToSave, &lRect, lTGAFormatSurface, &lRect);

  SDL_LockSurface(lTGAFormatSurface);
  // Write Sapphire's signature to the file.
  fwrite(TGA::TurboSphereID, 1, TGA::IDSize, lFile);

  WriteMatchesV(lFile, lTGAFormatSurface->pixels, (uint8_t *)lTGAFormatSurface->pixels+(lDimensions[0]*lDimensions[1]*4));

  //fwrite(lTGAFormatSurface->pixels, 4, lDimensions[0]*lDimensions[1], lFile);

  fflush(lFile);
  fclose(lFile);
  SDL_UnlockSurface(lTGAFormatSurface);
  SDL_FreeSurface(lTGAFormatSurface);
}

}
}
