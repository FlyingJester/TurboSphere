#include "surface.h"

inline int min(int a, int b){
    if(a>b)
        return a;
    return b;
}

// Surface Queued
int TS_SurfaceGradientLine(SDL_Surface *destination, int x[2], int y[2], RGBA color[2]) {

    int ax1 = x[0];
    int ay1 = y[0];
    int ax2 = x[1];
    int ay2 = y[1];

    int w = abs(ax1-ax2);
    int h = abs(ay1-ay2);

    SDL_Rect rect = {(short int)min(ax1, ax2), (short int)min(ay1, ay2), (short unsigned int)w, (short unsigned int)h};

    int x1 = ax1-min(ax1, ax2);
    int y1 = ay1-min(ay1, ay2);
    int x2 = ax2-min(ax1, ax2);
    int y2 = ay2-min(ay1, ay2);

    SDL_Rect temprect = {(short int)x1, (short int)y1, 1, 1};

    SDL_Surface *surface = SDL_CreateRGBSurface(0, w+1, h+1, 32, CHANNEL_MASKS);

    int sx;
    int sy;
    if (x1<x2) {
        sx = 1;
    }
    else {
        sx = -1;
    }
    if (y1<y2) {
        sy = 1;
    }
    else {
        sy = -1;
    }
    float err = (float)w-h;

    float e2;
    while(true) {
        temprect.x = x1;
        temprect.y = y1;

        SDL_FillRect(surface, &temprect, *color);

        if ((x1==x2)&&(y1==y2)) {
            break;
        }
        e2 = 2*err;

        if (e2>-h) {

            err-= h;
            x1 += sx;

        }
        if (e2<w) {

            err += w;
            y1 += sy;

        }
    }

    SDL_BlitSurface(surface, NULL, destination, &rect);
    SDL_FreeSurface(surface);

    return 1;

}
// Surface Queued
int TS_SurfaceGradientTriangle(SDL_Surface *surf, int x[3], int y[3], RGBA color[3]) {

    return 1;
}
// Surface Queued
int TS_SurfaceOutlinedEllipse(SDL_Surface *surf, int x, int y, unsigned int radius, RGBA color) {

    return 1;
}
// Surface Queued
int TS_SurfaceGradientEllipse(SDL_Surface *surf, int x, int y, unsigned int radius, RGBA color[2]) {

    return 1;
}
