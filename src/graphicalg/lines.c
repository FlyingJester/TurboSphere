#define GRAPHICALG_INTERNAL
#include "lines.h"
#include "stdlib.h"
#include <assert.h>
#include <stdio.h>
#include <math.h>

TS_Point *CalcLine(const TS_Point /*in*/ aLine[2], int * /*out*/oSteps){

    struct TS_Point *rPoints = NULL;

    int lw = (aLine[0].x - aLine[1].x);
    int lh = (aLine[0].y - aLine[1].y);
    if(lw < 0) lw*=-1;
    if(lh < 0) lh*=-1;

    const int w = lw;
    const int h = lh;

    assert(w >= 0);
    assert(h >= 0);

        rPoints = malloc(sizeof(struct TS_Point)*(w+h));
        int x1 = aLine[0].x;
        int y1 = aLine[0].y;
        int x2 = aLine[1].x;
        int y2 = aLine[1].y;

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
            rPoints[i].y = y1;

            if ((x1==x2)&&(y1==y2))
              break;

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

    *oSteps = i-1;

    return rPoints;

}
