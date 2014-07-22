#include "save.h"
#ifndef _MSC_VER
#include <png.h>
#else
#include "include/png.h"
#endif
#include "../common/plugin.h"
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <cctype>

#include "main.h"

//These are hardcoded at compile time.
//I support the alpha channel, always.
//#define BPP 4
//#define DEPTH 8

int save_AUTO(const char * path, const void * pixels, unsigned int width, unsigned int height){

    size_t len = strlen(path);
    char * lowerPath = (char *)(((intptr_t)path)+len-4);
    lowerPath[1] = tolower(lowerPath[1]);
    lowerPath[2] = tolower(lowerPath[2]);
    lowerPath[3] = tolower(lowerPath[3]);
    if(strcmp(lowerPath, ".png")==0&&false){
        return save_PNG(path, pixels, width, height, 0);
    }
    else if(strcmp(lowerPath, ".tga")==0){
        return save_TGA(path, pixels, width, height, R8G8B8A8, SDL_GL_SAVETGA_COMPRESS|SDL_GL_SAVETGA_RGBA);
    }
    else {
        SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(const_cast<void *>(pixels), width, height, DEPTH, BPP*width, CHANNEL_MASKS);
        int err = SDL_SaveBMP(surface, path);
        SDL_FreeSurface(surface);
        if(err!=0)
            return SDL_GL_SAVE_UNKNOWN;
    }
    return SDL_GL_SAVE_NOERROR;
}

int save_PNG(const char * path, const void *pixels, unsigned int width, unsigned int height, char flags){
    FILE * file;
    png_structp pngs = NULL;
    png_infop   info = NULL;

    file = fopen(path, "wb");
    if(!file)
        return SDL_GL_SAVE_NOFILE;

	if((width|height)==0){
		return SDL_GL_SAVE_NOERROR;
	}

	setjmp(png_jmpbuf(pngs));
    pngs = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info = png_create_info_struct(pngs);
    if((!pngs)||(!info)){
        fclose(file);
        return SDL_GL_SAVE_PNGERROR;
    }

	setjmp(png_jmpbuf(pngs));
    png_set_IHDR(pngs, info, width, height, DEPTH, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_byte **rowlist = (png_byte **)png_malloc(pngs, height*sizeof(png_byte*));
    for(unsigned int y = 0; y<height; y++){
        png_byte *row = (png_byte*)png_malloc(pngs, BPP*width);
        rowlist[y] = row;
        memcpy(row, (char *)pixels+(width*y*BPP), width*BPP);
    }
	setjmp(png_jmpbuf(pngs));
    png_init_io (pngs, file);
	setjmp(png_jmpbuf(pngs));
    png_set_rows(pngs, info, rowlist);
	setjmp(png_jmpbuf(pngs));
    png_write_png(pngs, info, PNG_TRANSFORM_IDENTITY, NULL);

    fclose(file);

    for(unsigned int i = 0; i<height; i++){
        png_free(pngs, rowlist[i]);
    }
    png_free(pngs, rowlist);

    png_destroy_write_struct(&pngs, &info);

    return SDL_GL_SAVE_NOERROR;
}

int save_TGA(const char * path, const void *pixels, unsigned int width, unsigned int height, TGA_format format, char flags){

    FILE *file;

    uint8_t ColorMap[5] = {0, 0, 0, 0, 0};


    uint8_t ImageDescriptor = (0x20);
    if(flags&SDL_GL_SAVETGA_CORONACOMPAT)
        ImageDescriptor|=9;
    else
        ImageDescriptor|=9;
    printf("Saving a TGA:\t%s", path);
    file = fopen(path, "wb");
    if(!file)
        return SDL_GL_SAVE_NOFILE;

    fputc(0, file); //ID Length
    fputc(0, file); //ColorMap Type
    if(flags&SDL_GL_SAVETGA_COMPRESS)
        fputc(10, file); //Image Type (RLE TrueColor = 10)
    else if(flags&SDL_GL_SAVETGA_CORONACOMPAT)
        fputc(2, file); //Image Type (TrueColor = 2)
    else
        fputc(10, file);

    fwrite(ColorMap, sizeof(uint8_t), sizeof(ColorMap), file); //ColorMap
    if(!(flags&SDL_GL_SAVETGA_GLNATIVE)){
        fseek(file, 10, SEEK_SET);
        fputc(height      &0xFF, file);
        fputc((height >>8)&0xFF, file);
    }
    else{
        fseek(file, 8, SEEK_SET);
        fputc(width       &0xFF, file);
        fputc((width  >>8)&0xFF, file);
        fseek(file, 2, SEEK_CUR);
    }
    fputc(width       &0xFF, file);
    fputc((width  >>8)&0xFF, file);
    fputc(height      &0xFF, file);
    fputc((height >>8)&0xFF, file);
    fseek(file, 16, SEEK_SET);
    fputc(DEPTH, file); // Depth
    fputc(ImageDescriptor, file); //Image Descriptor (pixel data orientation)

    if(flags&SDL_GL_SAVETGA_COMPRESS){

    //NOTE:
    //For simplicity, we only chunk same-data chunks. Every time a dissimilar pixel is found, we start a new chunk.

        char chunk[5];
        size_t pixelNum = 0;
        while(pixelNum<width*height){
            size_t i = pixelNum+1;
            if((0x7F<width*height)&&(pixelNum+0x7F>=width*height))
                break;

            int newPix = pixelNum/width; //Current row number.
            newPix-=height-1;
            newPix*=-1;
            newPix*=width;
            newPix+=(pixelNum%width);

            if(flags&SDL_GL_SAVETGA_GLNATIVE){
                //i = newPix+1;
                while((i<width*height)&&(i-newPix<0x7F)&&(i%width!=0)&&(memcmp((char *)pixels+(newPix*BPP), (char *)pixels+(i*BPP), BPP)==0)){
                    i++;
                }
            }
            else{
                while((i<width*height)&&(i-pixelNum<0x7F)&&(i%width!=0)&&(memcmp((char *)pixels+(pixelNum*BPP), (char *)pixels+(i*BPP), BPP)==0)){
                    i++;
                }
            }

            if(i!=pixelNum+1)
                chunk[0]=0x80|(i-pixelNum-1);
            else
                chunk[0]=0;

            if(flags&SDL_GL_SAVETGA_GLNATIVE){

                if(flags&SDL_GL_SAVETGA_RGBA){
                    memcpy(chunk+1, ((char *)pixels+((newPix*BPP)+2)), 1);
                    memcpy(chunk+2, ((char *)pixels+((newPix*BPP)+1)), 1);
                    memcpy(chunk+3, ((char *)pixels+((newPix*BPP)+0)), 1);
                    memcpy(chunk+4, ((char *)pixels+((newPix*BPP)+3)), 1);
                }
                else if(flags&SDL_GL_SAVETGA_BGRA){
                    memcpy(chunk+1, (char *)pixels+(newPix*BPP), 4);
                }
            }
            else{
                if(flags&SDL_GL_SAVETGA_RGBA){
                    memcpy(chunk+1, ((char *)pixels+((pixelNum*BPP)+2)), 1);
                    memcpy(chunk+2, ((char *)pixels+((pixelNum*BPP)+1)), 1);
                    memcpy(chunk+3, ((char *)pixels+((pixelNum*BPP)+0)), 1);
                    memcpy(chunk+4, ((char *)pixels+((pixelNum*BPP)+3)), 1);
                }
                else if(flags&SDL_GL_SAVETGA_BGRA){
                    memcpy(chunk+1, (char *)pixels+(pixelNum*BPP), 4);
                }
            }
            fwrite(chunk, 1, sizeof(chunk), file);

            pixelNum=i;
        }

        //Slight screw up with the previous.
        //TODO: Fix this so the last chunk of the file is RLE'd as well, and the previous loop does NOT increment pixelNum one too many times.
        pixelNum--;
        //fflush(file);
        //Finish the writing.

        for (chunk[0]=0; pixelNum<width*height; pixelNum++){

                if(flags&SDL_GL_SAVETGA_RGBA){
                    memcpy(chunk+1, ((char *)pixels+((pixelNum*BPP)+2)), 1);
                    memcpy(chunk+2, ((char *)pixels+((pixelNum*BPP)+1)), 1);
                    memcpy(chunk+3, ((char *)pixels+((pixelNum*BPP)+0)), 1);
                    memcpy(chunk+4, ((char *)pixels+((pixelNum*BPP)+3)), 1);
                }
                else if(flags&SDL_GL_SAVETGA_BGRA){
                    memcpy(chunk+1, (char *)pixels+(pixelNum*BPP), 4);
                }

            fwrite(chunk, 1, sizeof(chunk), file);
        }
        fflush(file);
    }
    else if(flags&SDL_GL_SAVETGA_CORONACOMPAT){
        for(unsigned int i = 0; i<width*height; i++){
            fwrite(((char *)pixels+((i*BPP)+2)), 1, 1, file);
            fwrite(((char *)pixels+((i*BPP)+1)), 1, 1, file);
            fwrite(((char *)pixels+((i*BPP))),   1, 1, file);
            fwrite(((char *)pixels+((i*BPP)+3)), 1, 1, file);
        }
    }

    fclose(file);

    return SDL_GL_SAVE_NOERROR;
}
