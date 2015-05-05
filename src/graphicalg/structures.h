#pragma once

#include "graphicalg_stub.h"

#include <stdint.h>
#include <stdlib.h>

#ifndef __cplusplus
typedef
#endif
struct TS_Point {

	int x;
	int y;

}
#ifndef __cplusplus
TS_Point
#endif
;

#ifndef __cplusplus
typedef
#endif
struct TS_Segment {

	int x1;
	int y1;
	int x2;
	int y2;

}
#ifndef __cplusplus
TS_Segment
#endif
;

GRAPHICALG_EXPORT TS_Point *  GRAPHICALGCALL TS_CreatePoint(int x, int y);
GRAPHICALG_EXPORT void        GRAPHICALGCALL TS_DeletePoint(TS_Point *aPoint);
GRAPHICALG_EXPORT TS_Segment *GRAPHICALGCALL TS_CreateSegment(int _x1, int _y1, int _x2, int _y2);
GRAPHICALG_EXPORT void        GRAPHICALGCALL TS_DeleteSegment(TS_Segment *aSegment);
