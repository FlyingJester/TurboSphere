#define GRAPHICALG_INTERNAL
#include "circles.h"
#include <math.h>
#include <assert.h>

TS_Point *TS_CalcLine(const TS_Segment */*in*/aLine, int * /*out*/ oSteps){

    TS_Point *rPoints = NULL;

    const unsigned w = DIFF(aLine->x1, aLine->x2);
    const unsigned h = DIFF(aLine->y1, aLine->y2);
    if(aLine->x1==aLine->x2){
        rPoints = malloc(sizeof(TS_Point)*h);
        for(int i = 0; i<h; i++){
            rPoints[i].x = aLine->x1;
            rPoints[i].y = (
                              (aLine->y1*i) +
                              (aLine->y2*(h-i))
                            ) / h;
        }
        *oSteps = h;

    }
    else if(aLine->y1==aLine->y2){
        rPoints = malloc(sizeof(TS_Point)*w);
        for(int i = 0; i<w; i++){
            rPoints[i].x = (
                              (aLine->x1*i) +
                              (aLine->x2*(w-i)
                              )
                           ) / w;
            rPoints[i].y = aLine->y1;
        }
        *oSteps = w;
    }
    else {
        rPoints = malloc(sizeof(TS_Point)*(w+h));
        int x1 = aLine->x1;
        int y1 = aLine->y1;
        int x2 = aLine->x2;
        int y2 = aLine->y2;

        int sx;
        int sy;
        if (x1<x2){
            sx = 1;
        }
        else {
            sx = -1;
        }
        if (y1<y2){
            sy = 1;
        }
        else{
            sy = -1;
        }
        float err = (float)(w-h);
        float e2;

        int i = 0;

        while(i<w+h){

            rPoints[i].x = x1;
            rPoints[i].x = y1;

            if ((x1==x2)&&(y1==y2)){break;}
            e2 = 2*err;

            if (e2>-h){

                err-= h;
                x1 += sx;

            }
            if (e2<w){

                err += w;
                y1 += sy;

            }

            i++;
        }
        *oSteps = i;
    }

    return rPoints;

}

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

int TS_DeleteLine(TS_Point *aDelete){
    free(aDelete);
    return 0;
}

int TS_DeleteCircle(TS_Point *aDelete){
    free(aDelete);
    return 0;
}
