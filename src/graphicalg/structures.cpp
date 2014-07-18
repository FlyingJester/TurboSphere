#define GRAPHICALG_INTERNAL
#include "structures.h"

TS_Point *TS_CreatePoint(int x, int y){
  TS_Point *rPoint = new TS_Point();
  rPoint->x = x;
  rPoint->y = y;
  return rPoint;
}

void TS_DeletePoint(TS_Point *aPoint){
  delete aPoint;
}

TS_Segment *TS_CreateSegment(int _x1, int _y1, int _x2, int _y2){
    TS_Segment *rSegment = new TS_Segment();
    rSegment->x1 = _x1;
    rSegment->y1 = _y1;
    rSegment->x2 = _x2;
    rSegment->y2 = _y2;
    return rSegment;
}

void TS_DeleteSegment(TS_Segment *aSegment){
  delete aSegment;
}
