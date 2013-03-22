#define GRAPHICALG_INTERNAL
#include "structures.h"
/*
TS_Point TS_Point::operator=(const SDL_Rect &c2)
{
    // use the Cents constructor and operator+(int, int)
    return TS_Point(c2.x, c2.y);
}
*/
TS_Point::TS_Point(int _x, int _y){
    x = _x;
    y = _y;
}

TS_Point::~TS_Point(){

}

TS_Segment::TS_Segment(int _x1, int _y1, int _x2, int _y2){
    x1 = _x1;
    y1 = _y1;
    x2 = _x2;
    y2 = _y2;
}

TS_Segment::TS_Segment(void){
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
}

TS_Segment::~TS_Segment(){

}
