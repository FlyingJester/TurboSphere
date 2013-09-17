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
        return SDL_GL_SAVE_NOFILE;

    pngs = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info = png_create_info_struct(pngs);
    if((!pngs)||(!info)){
        fclose(file);
        return SDL_GL_SAVE_PNGERROR;
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

    return SDL_GL_SAVE_NOERROR;
}

int save_TGA(const char * path, void *pixels, unsigned int width, unsigned int height, TGA_format format, char flags){

    FILE *file;


    uint8_t ID_Length = 0;
    uint8_t ColorMapType = 0;
    uint8_t ImageType = 10;
    uint8_t ColorMap[5] = {0, 0, 0, 0, 0};
    uint8_t Xorigin = 0;
    uint8_t Yorigin = height;
    uint8_t Width = width;
    uint8_t Height = height;
    uint8_t PixelDepth;

    switch (format){
        case (R8G8B8):
            PixelDepth = 24;
            break;
        case (R8G8B8A8):
            PixelDepth = 32;
            break;
        case (R5G5B5A1):
            PixelDepth = 16;
            break;
        default:
            PixelDepth = 32;
    }

    uint8_t ImageDescriptor = (0x20);
    if(flags&SDL_GL_SAVETGA_CORONACOMPAT)
        ImageDescriptor|=9;
    else
        ImageDescriptor|=4;



    file = fopen(path, "wb");
    if(!file)
        return SDL_GL_SAVE_NOFILE;

    fputc(0, file); //ID Length
    fputc(0, file); //ColorMap Type
    if(flags&SDL_GL_SAVETGA_COMPRESS)
        fputc(10, file); //Image Type (RLE TrueColor = 10)
    else
        fputc(2, file); //Image Type (TrueColor = 2)

    fwrite(ColorMap, sizeof(uint8_t), sizeof(ColorMap), file); //ColorMap
    fseek(file, 10, SEEK_SET);
    fputc(height      &0xFF, file);
    fputc((height >>8)&0xFF, file);
    fputc(width       &0xFF, file);
    fputc((width  >>8)&0xFF, file);
    fputc(height      &0xFF, file);
    fputc((height >>8)&0xFF, file);
    fseek(file, 16, SEEK_SET);
    fputc(PixelDepth, file); // Depth
    fputc(ImageDescriptor, file); //Image Descriptor (pixel data orientation)

    if(flags&SDL_GL_SAVETGA_COMPRESS){

        char chunk[5];
        size_t pixelNum = 0;
        while(pixelNum<width*height){
            size_t i = pixelNum+1;
            while((i<width*height)&&(i-pixelNum<0x0F)&&(i-pixelNum%width)&&(memcmp(pixels+(pixelNum*BPP), pixels+(i*BPP), BPP)==0)){
                i++;
            }

            if(i!=pixelNum+1)
                chunk[0]=0x80|(i-pixelNum-1);
            else
                chunk[0]=0;

            memcpy(chunk+1, (pixels+((pixelNum*BPP)+2)), 1);
            memcpy(chunk+2, (pixels+((pixelNum*BPP)+1)), 1);
            memcpy(chunk+3, (pixels+((pixelNum*BPP)+0)), 1);
            memcpy(chunk+4, (pixels+((pixelNum*BPP)+3)), 1);

            fwrite(chunk, 1, sizeof(chunk), file);

            pixelNum=i;

        }
    }
    else if(flags&SDL_GL_SAVETGA_CORONACOMPAT){
        for(int i = 0; i<width*height; i++){
            fwrite((pixels+((i*BPP)+2)), 1, 1, file);
            fwrite((pixels+((i*BPP)+1)), 1, 1, file);
            fwrite((pixels+((i*BPP))),   1, 1, file);
            fwrite((pixels+((i*BPP)+3)), 1, 1, file);
        }
    }

    fclose(file);

    return SDL_GL_SAVE_NOERROR;
}
