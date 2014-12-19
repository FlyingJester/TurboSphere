#ifndef USE_JPEG
#error
#endif

#include <csetjmp>

extern "C"{
#include "jpeglib.h"
}

namespace Sapphire{
namespace Save{

namespace JPEG{

  class FreeSurface{
  public:
    void operator () (SDL_Surface * a) {printf(BRACKNAME " Info: Freeing temporary surface.\n");SDL_FreeSurface(a); a=(SDL_Surface *)0xBADF158;}
  };

  static jmp_buf JpegBuf;

  void longjmp(j_common_ptr){
    fprintf(stderr, BRACKNAME " SaveJPEG Error: Failed to save image.\n");
    std::longjmp(JpegBuf, 0xFF);
  }

  void emit_message(j_common_ptr, int msg_level){
    fprintf(stderr, BRACKNAME " SaveJPEG Info: Error of level %i emitted.\n", msg_level);
  }

  void output_message(j_common_ptr){
    fprintf(stderr, BRACKNAME " SaveJPEG Info: Error.\n");
  }

  std::unique_ptr<jpeg_compress_struct> compress_struct(new jpeg_compress_struct());
  std::unique_ptr<jpeg_error_mgr> jpegerror_struct(new jpeg_error_mgr());
  const uint32_t rmask = 0x0000FF;
  const uint32_t gmask = 0x00FF00;
  const uint32_t bmask = 0xFF0000;

}

SaveStatus JPEGSaveFunction(SDL_Surface *aToSave, const std::string &aPath);

SaveStatus JPEGInitFunction(void){
  int First = 1;
  if(First){
    fprintf(stderr, BRACKNAME " Info: Intializing JPEG settings.\n");

    JPEG::compress_struct->err = jpeg_std_error(JPEG::jpegerror_struct.get());

    JPEG::jpegerror_struct->error_exit     = JPEG::longjmp;
    JPEG::jpegerror_struct->emit_message   = JPEG::emit_message;
    JPEG::jpegerror_struct->output_message = JPEG::output_message;

    jpeg_create_compress(JPEG::compress_struct.get());
    First = 0;
  }

  SaveFunctions[Formats::jpeg] = JPEGSaveFunction;
  return ssSuccess;
}

SaveStatus JPEGSaveFunction(SDL_Surface *aToSave, const std::string &aPath){

    if(setjmp(JPEG::JpegBuf))
      return ssFailure;

    jpeg_compress_struct *lCompress = JPEG::compress_struct.get();

    assert(lCompress);

    FILE *lFile = fopen(aPath.c_str(), "wb");

    if(!lFile)
      return ssFailure;

    std::unique_ptr<FileHolder> lFileKill = std::unique_ptr<FileHolder>(new FileHolder(lFile));

    jpeg_stdio_dest(lCompress, lFile);

    printf(BRACKNAME " %s Info: Set jpeg file destination.\n", __func__);

    if(setjmp(JPEG::JpegBuf))
      return ssFailure;


    SDL_Surface *lJPEGFormatSurface = SDL_CreateRGBSurface(0, aToSave->w, aToSave->h, 24, JPEG::rmask, JPEG::gmask, JPEG::bmask, 0);
    SDL_Rect lRect = {0, 0, aToSave->w, aToSave->h};
    SDL_LowerBlit(aToSave, &lRect, lJPEGFormatSurface, &lRect);

    lCompress->image_width = lJPEGFormatSurface->w;
    lCompress->image_height = lJPEGFormatSurface->h;
    lCompress->input_components = lJPEGFormatSurface->format->BytesPerPixel;
    lCompress->in_color_space = JCS_RGB;

    printf(BRACKNAME " %s Info: Set jpeg info.\n", __func__);

    jpeg_set_defaults(lCompress);

//    jpeg_set_quality(lCompress, 100, TRUE);

    jpeg_start_compress(lCompress, TRUE);

    if(setjmp(JPEG::JpegBuf))
      return ssFailure;

    printf(BRACKNAME " %s Info: Writing jpeg.\n", __func__);
    // Lock the surface, unlock it when we return.

    std::unique_ptr<GenericHolder<SDL_Surface, JPEG::FreeSurface > >
        lSurfaceHolder(new GenericHolder<SDL_Surface, JPEG::FreeSurface > (lJPEGFormatSurface));
    SDL_UnlockSurface(lJPEGFormatSurface);

    SDL_SaveBMP(lJPEGFormatSurface, (aPath+std::string("1")).c_str());
    uint8_t *lPixels = (uint8_t *)lJPEGFormatSurface->pixels;


    for(int i = 0; i < lJPEGFormatSurface->h; i++){

      if(setjmp(JPEG::JpegBuf))
        return ssFailure;

      uint8_t *Row = (uint8_t *)&(lPixels[ i * lJPEGFormatSurface->w  * lJPEGFormatSurface->format->BytesPerPixel]);
      jpeg_write_scanlines(lCompress, &Row, 1);
    }

    if(setjmp(JPEG::JpegBuf))
      return ssFailure;

    jpeg_finish_compress(lCompress);

    fflush(lFile);

    return ssSuccess;

}

}
}
