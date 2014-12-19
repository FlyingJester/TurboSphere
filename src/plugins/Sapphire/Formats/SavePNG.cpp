#ifndef USE_PNG
#error
#endif

#ifdef NDEBUG
  #define debug_print(...)
#else
  #define debug_print(...) printf(__VA_ARGS__)
#endif
#include <memory>
#include <cstdio>
#include <setjmp.h>

#include <TSPR/dlopenwrap.h>

namespace Sapphire {
namespace Save {

  typedef void *png_structp;
  typedef void *png_infop;
  typedef uint8_t png_byte;
  typedef png_byte* png_bytep;

#  define png_jmpbuf(png_ptr) \
      (png_set_longjmp_fn((png_ptr), longjmp, (sizeof (jmp_buf))))

  int * (*png_set_longjmp_fn)(png_structp, void(*)(jmp_buf, int), size_t) = nullptr;
  void   (*png_init_io )(png_structp, FILE *) = nullptr;
  void   (*png_set_rows)(png_structp, png_infop, png_byte **) = nullptr;
  void   (*png_set_IHDR)(png_structp, png_infop, uint32_t, uint32_t, int, int, int, int, int) = nullptr;
  png_structp(*png_create_write_struct)(const char *, void *, void *, void *) = nullptr;
  png_infop(*png_create_info_struct)(png_structp) = nullptr;
  void   (*png_write_png)(png_structp, png_infop, int, void *) = nullptr;
  void   (*png_destroy_write_struct)(png_structp*, png_infop*) = nullptr;

  const int PNG_COLOR_TYPE_RGB_ALPHA = 0x2|0x4;
  const int PNG_INTERLACE_NONE       = 0x0;
  const int PNG_INTERLACE_ADAM7      = 0x1;
  const int PNG_COMPRESSION_TYPE_BASE= 0x0;
  const int PNG_TRANSFORM_IDENTITY   = 0x0;
  const int PNG_FILTER_TYPE_BASE     = 0x0;
  const char *PNG_LIBPNG_VER_STRING  = "1.6.10";


SaveStatus PNGSaveFunction(SDL_Surface *aToSave, const std::string &aPath){

    debug_print( BRACKNAME " : Saving PNG %s, %ix%i\n", aPath.c_str(), aToSave->w, aToSave->h);

    assert(aToSave);
    assert(!aPath.empty());

    FILE * file;
    png_structp pngs = NULL;
    png_infop info = NULL;

    file = fopen(aPath.c_str(), "wb");
    if(!file)
      return ssFailure;

    std::unique_ptr<FileHolder> lFileKill = std::unique_ptr<FileHolder>(new FileHolder(file));

    pngs = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    info = png_create_info_struct(pngs);
    if((!pngs)||(!info))
        return ssFailure;

    if(setjmp(png_jmpbuf(pngs)))
      return ssFailure;

    png_set_IHDR(pngs, info, aToSave->w, aToSave->h, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_ADAM7, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_bytep*rowlist = new png_bytep [aToSave->h*sizeof(png_byte*)];

    std::unique_ptr<ArrayHolder<png_bytep> > lRowsKill = std::unique_ptr<ArrayHolder<png_bytep> >(new ArrayHolder<png_bytep> (rowlist) );
    std::unique_ptr<SurfaceHolder> lSurfaceLock = std::unique_ptr<SurfaceHolder>(new SurfaceHolder(aToSave));

    for(unsigned int y = 0; y<aToSave->h; y++){
        rowlist[y] = static_cast<uint8_t *>(aToSave->pixels)+(aToSave->w*y*4);
    }

    if(setjmp(png_jmpbuf(pngs)))
      return ssFailure;

    png_init_io(pngs, file);

    if(setjmp(png_jmpbuf(pngs)))
      return ssFailure;

    png_set_rows(pngs, info, rowlist);

    if(setjmp(png_jmpbuf(pngs)))
      return ssFailure;

    png_write_png(pngs, info, PNG_TRANSFORM_IDENTITY, nullptr);

    png_destroy_write_struct(&pngs, &info);

    return ssSuccess;
}

SaveStatus PNGInitFunction(void){

  debug_print(BRACKNAME " Info: Initializing PNG save subsystem.\n");

  void *LibPng = DLOPENSYSLIBRARY("png", DL_LAZY|DL_LOCAL);

  if(!LibPng){
    fprintf(stderr, BRACKNAME "Couldn't load libpng.\n");
    return ssFailure;
  }

  printf(BRACKNAME " Info: Loaded libpng.\n");

  png_set_longjmp_fn = (int *(*)(png_structp, void(*)(jmp_buf, int), size_t))DLOPENFUNCTION(LibPng, "png_set_longjmp_fn");
  //png_jmpbuf   = (int *(*)(png_structp))DLOPENFUNCTION(LibPng, "png_jmpbuf");//  int *(*png_jmpbuf  )(png_structp)         = nullptr;
  png_init_io  = (void (*)(png_structp, FILE *))DLOPENFUNCTION(LibPng, "png_init_io");
  png_set_rows = (void (*)(png_structp, png_infop, png_bytep*))DLOPENFUNCTION(LibPng, "png_set_rows");
  png_set_IHDR = (void (*)(png_structp, png_infop, uint32_t, uint32_t, int, int, int, int, int))DLOPENFUNCTION(LibPng, "png_set_IHDR");
  png_create_write_struct = (png_structp(*)(const char *, void *, void *, void *))DLOPENFUNCTION(LibPng, "png_create_write_struct");
  png_create_info_struct  = (png_infop(*)(png_structp))DLOPENFUNCTION(LibPng, "png_create_info_struct");
  png_write_png= (void (*)(png_structp, png_infop, int, void *))DLOPENFUNCTION(LibPng, "png_write_png");
  png_destroy_write_struct = (void(*)(png_structp*, png_infop*))DLOPENFUNCTION(LibPng, "png_destroy_write_struct");

  if( (png_set_longjmp_fn)
    &&(png_init_io)
    &&(png_set_rows)
    &&(png_set_IHDR)
    &&(png_create_write_struct)
    &&(png_create_info_struct)
    &&(png_write_png)
    &&(png_destroy_write_struct)
  ){
    SaveFunctions[Formats::png] = PNGSaveFunction;

    debug_print(BRACKNAME " Info: Can save PNGs.\n");

    return ssSuccess;
  }
  else{

    printf(BRACKNAME " Error: ugly dump comin' your way:\n\t%p\n\t%p\n\t%p\n\t%p\n\t%p\n\t%p\n\t%p\n\t%p.\n"
    , (png_set_longjmp_fn)
    , (png_init_io)
    , (png_set_rows)
    , (png_set_IHDR)
    , (png_create_write_struct)
    , (png_create_info_struct)
    , (png_write_png)
    , (png_destroy_write_struct)
      );
  }

  SaveFunctions[Formats::png] = nullptr;
  return ssFailure;
}


}
}
