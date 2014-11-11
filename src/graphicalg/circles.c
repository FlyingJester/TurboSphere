#define GRAPHICALG_INTERNAL
#include "circles.h"
#include <math.h>
#include <assert.h>

TS_Point *TS_CalcCircle(int x, int y, int r, int steps){
    TS_Point *rPoints = malloc(sizeof(TS_Point)*steps);

    assert(sizeof(TS_Point)==(sizeof(int)*2));

    TS_CalcCircleInto(x, y, r, steps, (int *)rPoints, 0);

    return rPoints;
}

int TS_CalcCircleInto(int x, int y, int r, int steps, int * /*out*/oInto, size_t stride){
    float angle = 0;


    float inc = (ALG_PI2)/((float)(steps+2));

    int i = 0;
    if(stride==0){
        while(i<steps){
            oInto[(i*2)+0] = ((((float)(cosf(angle)))*(r))+0.5f)+x;
            oInto[(i*2)+1] = ((((float)(sinf(angle)))*(r))+0.5f)+y;
            angle+=inc;
            i++;
        }
    }
    else {
        int e = 0;
        while(i<steps){
            oInto[(e*2)+0] = ((((float)(cosf(angle)))*(r))+0.5f)+x;
            oInto[(e*2)+1] = ((((float)(sinf(angle)))*(r))+0.5f)+y;
            angle+=inc;
            i++;
            e++;
            e+=stride;
        }
    }
    return steps;
}

int TS_CalcCircleIntoFloat(int x, int y, int r, int steps, float * /*out*/oInto, size_t stride){
    float angle = 0;

    float inc = (ALG_PI2)/((float)(steps+2));

    int i = 0;
    if(stride==0){
        while(i<steps){
            oInto[(i*2)+0] = ((((float)(cosf(angle)))*(r))+0.5f)+x;
            oInto[(i*2)+1] = ((((float)(sinf(angle)))*(r))+0.5f)+y;
            angle+=inc;
            i++;
        }
    }
    else {
        int e = 0;
        while(i<steps){
            oInto[(e*2)+0] = ((((float)(cosf(angle)))*(r))+0.5f)+x;
            oInto[(e*2)+1] = ((((float)(sinf(angle)))*(r))+0.5f)+y;
            angle+=inc;
            i++;
            e++;
            e+=stride;
        }
    }
    return steps;
}

int TS_DeleteCircle(TS_Point *aDelete){
    free(aDelete);
    return 0;
}
