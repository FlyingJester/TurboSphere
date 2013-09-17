#include "save.h"
#include <png.h>
#include <cstdlib>
#include <cstdint>
#include <cstdio>

//These are hardcoded at compile time.
//I support the alpha channel, always.
#define BPP 4
#define DEPTH 8


int save_PNG(const char * path, void *pixels, unsigned int width, unsigned int height, char flags){
    FILE * file;
    png_structp pngs  = NULL;
    png_infop   info = NULL;

    file = fopen(path, "wb");
    if(!file)
        return SDL_GL_SAVEPNG_NOFILE;

    pngs = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info = png_create_info_struct(pngs);
    if((!pngs)||(!info)){
        fclose(file);
        return SDL_GL_SAVEPNG_PNGERROR;
    }
    setjmp(png_jmpbuf(pngs));

    png_set_IHDR(pngs, info, width, height, DEPTH, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_ADAM7, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_byte **rowlist = (png_byte **)png_malloc(pngs, height*sizeof(png_byte*));
    for(int y = 0; y<height; y++){
        png_byte *row = (png_byte*)png_malloc(pngs, BPP*width);
        rowlist[y] = row;
        memcpy(row, pixels+(width*y*BPP), width*BPP);
    }

    png_init_io (pngs, file);
    png_set_rows(pngs, info, rowlist);
    png_write_png(pngs, info, PNG_TRANSFORM_IDENTITY, NULL);

    fclose(file);

    for(int i = 0; i<height; i++){
        png_free(pngs, rowlist[i]);
    }
    png_free(pngs, rowlist);

    png_destroy_write_struct(&pngs, &info);

    return SDL_GL_SAVEPNG_NOERROR;
}
