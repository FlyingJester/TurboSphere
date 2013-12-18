#define GRAPHIC_INTERNAL
#include "main.h"
#include "primitives.h"
#include "image.h"
#include "color.h"
#include "surface.h"
#include <sstream>
#include <assert.h>
#include "../../SDL2/SDL_opengl.h"


#ifdef USE_AMDLIBM

    #define sin sin_override
    #define cos cos_override
    #define floor floor_override
    #define ceil  ceil_override
    #define min min_override
    #define max max_override
    #define sqrt sqrt_override
#endif


void TS_SoftRectangle(int x, int y, int w, int h, TS_Color *c, SDL_Surface *destination){

    uint32_t color = c->toInt();

    SDL_Rect rect = {(short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h};
    SDL_Rect locrect = {0, 0, (short unsigned int)w, (short unsigned int)h};

	SDL_Surface* surface = SDL_CreateRGBSurface(0, w, h, DEPTH, CHANNEL_MASKS);
	SDL_FillRect(surface, &locrect, color);
	SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);
}


v8Function OutlinedCircle(V8ARGS);

void TS_SoftLine(void *arg){
    SoftLineArgs *carg = (SoftLineArgs*)arg;
    int ax1 = carg->x1;
    int ay1 = carg->y1;
    int ax2 = carg->x2;
    int ay2 = carg->y2;

    TS_Color *c = carg->color;
    SDL_Surface *destination = carg->surface;

    free(carg);

    uint32_t color = c->toInt();

    int w = abs(ax1-ax2);
    int h = abs(ay1-ay2);

    SDL_Rect rect = {(short int)min(ax1, ax2), (short int)min(ay1, ay2), (short unsigned int)w, (short unsigned int)h};

    if(ax1-ax2<=1&&ax1-ax2>=-1){
        SDL_Surface *surface = SDL_CreateRGBSurface(0, 1, h, DEPTH, CHANNEL_MASKS);
        SDL_Rect allsurf = {0, 0, 1, (short unsigned int)h};
        SDL_FillRect(surface, &allsurf, color);

        SDL_BlitSurface(surface, NULL, destination, &rect);
        SDL_FreeSurface(surface);

        return;
    }
    else if(ay1-ay2<=1&&ay1-ay2>=-1){
        SDL_Surface *surface = SDL_CreateRGBSurface(0, w, 1, DEPTH, CHANNEL_MASKS);
        SDL_Rect allsurf = {0, 0, (short unsigned int)w, 1};
        SDL_FillRect(surface, &allsurf, color);

        SDL_BlitSurface(surface, NULL, destination, &rect);
        SDL_FreeSurface(surface);

        return;
    }

    int x1 = ax1-min(ax1, ax2);
    int y1 = ay1-min(ay1, ay2);
    int x2 = ax2-min(ax1, ax2);
    int y2 = ay2-min(ay1, ay2);

    SDL_Rect temprect = {(short int)x1, (short int)y1, 1, 1};

	SDL_Surface *surface = SDL_CreateRGBSurface(0, w+1, h+1, DEPTH, CHANNEL_MASKS);

    int sx;
    int sy;
    if (x1<x2){
        sx  = 1;
    }
    else {
        sx  = -1;
    }
    if (y1<y2){
        sy  = 1;
    }
    else{
        sy  = -1;
    }
    float err = (float)w-h;

    float e2;
    while(true){
        temprect.x = x1;
        temprect.y = y1;

        SDL_FillRect(surface, &temprect, color);

        if ((x1==x2)&&(y1==y2)){break;}
        e2 = 2*err;

        if (e2>-h){

            err-= h;
            x1 += sx;

        }
        if (e2<w){

            err += w;
            y1  += sy;

        }
    }

	SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);
}

//Labelled horiz and vert for the direction in which the color changes.

void TS_SoftHorizGradRectangle(int x, int y, int w, int h, TS_Color *c1, TS_Color *c2, SDL_Surface *destination){
    SDL_Rect rect       = {(short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h};
    SDL_Rect temprect   = {0, 0, 1, (short unsigned int)h};
    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);

    TS_Color *color =  new TS_Color(c1->red, c1->green, c1->blue, c1->alpha);

    uint8_t redDif = c1->red  - c2->red;
    uint8_t grnDif = c1->green- c2->green;
    uint8_t bluDif = c1->blue - c2->blue;
    uint8_t alpDif = c1->alpha- c2->alpha;

    uint_fast32_t step = 0;

    for (int i = 0; i<w; i++){

        if(i!=0){
            step = i/w;
        }
        else{
            step = 0;
        }
        temprect.x = i;
        color->red   = c1->red  + (redDif*step);
        color->green = c1->green+ (grnDif*step);
        color->blue  = c1->blue + (bluDif*step);
        color->alpha = c1->alpha+ (alpDif*step);
        SDL_FillRect(surface, &temprect, color->toInt());
    }
	SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);
    delete color;
}

void TS_SoftVertGradRectangle(int x, int y, int w, int h, TS_Color *c1, TS_Color *c2, SDL_Surface *destination){
    SDL_Rect rect       = {(short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h};
    SDL_Rect temprect   = {0, 0, (unsigned short int)w, 1};
    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);

    TS_Color *color =  new TS_Color(c1->red, c1->green, c1->blue, c1->alpha);

    uint8_t redDif = c1->red  - c2->red;
    uint8_t grnDif = c1->green- c2->green;
    uint8_t bluDif = c1->blue - c2->blue;
    uint8_t alpDif = c1->alpha- c2->alpha;

    uint_fast32_t step = 0;

    for (int i = 0; i<h; i++){

        if(i!=0){
            step = i/h;
        }
        else{
            step = 0;
        }
        temprect.y = i;
        color->red   = c1->red  + (redDif*step);
        color->green = c1->green+ (grnDif*step);
        color->blue  = c1->blue + (bluDif*step);
        color->alpha = c1->alpha+ (alpDif*step);
        SDL_FillRect(surface, &temprect, color->toInt());
    }
	SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);
    delete color;
}

void TS_SoftGradientRectangle(int x, int y, int w, int h, TS_Color *c1, TS_Color *c2, TS_Color *c3, TS_Color *c4, SDL_Surface *destination){


    if(
        ((c1->toInt()==c2->toInt())||((c1->alpha==0)&&(c2->alpha==0))) &&
        ((c3->toInt()==c4->toInt())||((c3->alpha==0)&&(c4->alpha==0)))
    ){
        TS_SoftVertGradRectangle(x, y, w, h, c1, c3, destination);
        return;
    }
    else if(
        ((c1->toInt()==c4->toInt())||((c1->alpha==0)&&(c4->alpha==0))) &&
        ((c3->toInt()==c2->toInt())||((c3->alpha==0)&&(c2->alpha==0)))
    ){
        TS_SoftHorizGradRectangle(x, y, w, h, c1, c2, destination);
        return;
    }

    SDL_Rect rect       = {(short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h};
    SDL_Rect temprect   = {0, 0, 1, 1};
    SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, w, h, DEPTH, CHANNEL_MASKS);

    TS_Color color(0, 0, 0, 0);

    int rDeltaTop = c2->red   - c1->red;
    int gDeltaTop = c2->green - c1->green;
    int bDeltaTop = c2->blue  - c1->blue;
    int aDeltaTop = c2->alpha - c1->alpha;

    int rDeltaLow = c3->red   - c4->red;
    int gDeltaLow = c3->green - c4->green;
    int bDeltaLow = c3->blue  - c4->blue;
    int aDeltaLow = c3->alpha - c4->alpha;

    color.red   = c1->red;
    color.green = c1->green;
    color.blue  = c1->blue;
    color.alpha = c1->alpha;

    int rLineDelta;
    int gLineDelta;
    int bLineDelta;
    int aLineDelta;

    TS_Color topColor = *c1;
    TS_Color lowColor = *c4;

    for(int tx = 0; tx<w; tx++){
        rLineDelta = lowColor.red   -topColor.red;
        gLineDelta = lowColor.green -topColor.green;
        bLineDelta = lowColor.blue  -topColor.blue;
        aLineDelta = lowColor.alpha -topColor.alpha;
        for(int ty = 0; ty<h; ty++){

            SDL_FillRect(surface, &temprect, color.toInt());
            color.red  =topColor.red  + (rLineDelta*ty/h);
            color.green=topColor.green+ (gLineDelta*ty/h);
            color.blue =topColor.blue + (bLineDelta*ty/h);
            color.alpha=topColor.alpha+ (aLineDelta*ty/h);
            temprect.y=ty;
        }

        topColor.red  =c1->red  + (rDeltaTop*tx/w);
        topColor.green=c1->green+ (gDeltaTop*tx/w);
        topColor.blue =c1->blue + (bDeltaTop*tx/w);
        topColor.alpha=c1->alpha+ (aDeltaTop*tx/w);

        lowColor.red  =c4->red  + (rDeltaLow*tx/w);
        lowColor.green=c4->green+ (gDeltaLow*tx/w);
        lowColor.blue =c4->blue + (bDeltaLow*tx/w);
        lowColor.alpha=c4->alpha+ (aDeltaLow*tx/w);

        temprect.x=tx;
    }

	SDL_BlitSurface(surface, NULL, destination, &rect);
	SDL_FreeSurface(surface);
}

void TS_SoftFilledCircle(int x, int y, int rad, TS_Color *c, SDL_Surface *destination){

    uint32_t color = c->toInt();

    //Repair poor parameters.
    if(rad<0){
        rad*=-1;
    }
    else if(rad>0){

    }
    else{
        return;
    }

    SDL_Surface* surface = SDL_CreateRGBSurface(0, rad*2, rad*2, DEPTH, CHANNEL_MASKS);

    int w = rad*2;
    double dis= 0;
    int xd2 = 0;
    int yd2 = 0;
    double r  = rad;

    int inf = (rad>4)?((rad>>3)+rad-floor((double)(((double)rad)/sqrt(2.0)))):(rad);

    SDL_Rect dest = {1, 1, 1, 1};

    for(int xt = 0; xt<=rad; xt++){
        xd2 = rad-xt;
        xd2*= xd2;
        for(int yt = 0; yt<inf; yt++){
            yd2 = rad-yt;
            yd2*= yd2;
            dis = floor(sqrt((float)(yd2+xd2)));
            if(dis<r){
                dest.x = yt;
                dest.y = xt;
                dest.w = (rad-yt)*2;
                SDL_FillRect(surface, &dest, color);

                dest.y=((rad*2)-dest.y);
                SDL_FillRect(surface, &dest, color);

                dest.x = xt;
                dest.y = yt;
                dest.w = (rad-xt)*2;
                SDL_FillRect(surface, &dest, color);

                dest.y=((rad*2)-dest.y);
                SDL_FillRect(surface, &dest, color);
                break;
            }
        }
    }

    SDL_Rect rect = {(short int)(x-rad), (short int)(y-rad), (short unsigned int)(w), (short unsigned int)(w)};
    SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);

}


void TS_SoftOutlinedCircle(int x, int y, int rad, TS_Color *c, SDL_Surface* destination){

    uint32_t color = c->toInt();

    //Repair poor parameters.
    if(rad<0){
        rad*=-1;
    }
    else if(rad>0){

    }
    else{
        return;
    }

    SDL_Surface* surface = SDL_CreateRGBSurface(0, rad*2, rad*2, DEPTH, CHANNEL_MASKS);

    Uint32 *field = (Uint32 *)surface->pixels;
    int w = rad*2;
    double dis= 0;
    int xd2 = 0;
    int yd2 = 0;
    double r  = rad;
    int inf = (rad>4)?((rad>>2)+rad-floor((double)(((double)rad)/sqrt(2.0)))):(rad);

    for(int xt = 0; xt<=rad; xt++){
        xd2 = rad-xt;
        xd2 *= xd2;
        for(int yt = 0; yt<inf; yt++){
            yd2 = rad-yt;
            yd2*= yd2;
            dis = floor(sqrt(double(yd2+xd2)));
            if(dis<r){
                field[(yt*surface->w)+xt] = color;
                field[((w-yt)*surface->w)+(w-xt)] = color;
                field[(xt*surface->w)+yt] = color;
                field[((w-xt)*surface->w)+(w-yt)] = color;

                field[((w-yt)*surface->w)+xt] = color;
                field[((w-xt)*surface->w)+yt] = color;
                field[(yt*surface->w)+(w-xt)] = color;
                field[(xt*surface->w)+(w-yt)] = color;
                break;
            }
        }
    }

    SDL_Rect rect = {(short int)(x-rad), (short int)(y-rad), (short unsigned int)(w), (short unsigned int)(w)};
    SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);

}

