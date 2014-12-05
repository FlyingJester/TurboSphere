#pragma once

#include "graphicalg_stub.h"
#include "structures.h"

GRAPHICALG_EXPORT int GRAPHICALGCALL TS_CalcCircleInto(int x, int y, int r, int steps, int * /*out*/oInto, size_t stride);
GRAPHICALG_EXPORT int GRAPHICALGCALL TS_CalcCircleIntoFloat(int x, int y, int r, int steps, float * /*out*/oInto, size_t stride);
GRAPHICALG_EXPORT TS_Point *GRAPHICALGCALL TS_CalcCircle(int x, int y, int r, int steps);
GRAPHICALG_EXPORT int GRAPHICALGCALL TS_DeleteCircle(TS_Point *aDelete);
