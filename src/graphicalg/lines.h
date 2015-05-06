#pragma once
#include "structures.h"

#ifdef __cplusplus
extern "C" {
#endif
GRAPHICALG_EXPORT TS_Point * GRAPHICALGCALL CalcLine(const TS_Point /*in*/ aLine[2], int * /*out*/oSteps);
#ifdef __cplusplus
}
#endif
