#ifndef USE_TIFF
#error
#endif
#include <tiffio.h>
namespace Sapphire{
namespace Save{

namespace TIFF{

}


SaveStatus TIFFInitFunction(void){

  return ssSuccess;
}

SaveStatus TIFFSaveFunction(SDL_Surface *aToSave, const std::string &aPath){

  TIFF *tif = TIFFOpen(aPath.c_str(),"w");
  TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, aToSave->w);
  TIFFSetField(tif, TIFFTAG_IMAGELENGTH, aToSave->h);
  TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
  TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, aToSave->format->BitsPerPixel);
  TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
  TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_SEPARATE);
  TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  TIFFSetField(tif, TIFFTAG_XRESOLUTION, aToSave->w);
  TIFFSetField(tif, TIFFTAG_YRESOLUTION, aToSave->h);
  TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, 1);

  for (i = 0; i < rows; i++){
    TIFFWriteScanline(tif, &dataArray[i * columns], i, 0);
  }
  TIFFClose(tif);

  return ssSuccess;
}



}
}

