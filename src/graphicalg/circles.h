#ifndef GRAPHICALG_CIRCLES_HEAD
#define GRAPHICALG_CIRCLES_HEAD

#include <stdint.h>
#include <stdlib.h>

#include <structures.h>

#define ALG_PI  3.14159265358979323846f
#define ALG_PI2 6.28318530717958647692f

#define DIFF(A, B) ((A>B)?\
          (A-B) :\
          (B-A) )

#ifndef GRAPHICALG_EXPORT
  #ifdef _WIN32
    #ifdef GRAPHICALG_INTERNAL
      #define GRAPHICALG_EXPORT __declspec(dllexport)
    #else
      #define GRAPHICALG_EXPORT __declspec(dllimport)
    #endif
  #define GRAPHICALGCALL _cdecl
  #else
    #define GRAPHICALGCALL
    #ifdef __cplusplus
      #define GRAPHICALG_EXPORT extern "C"
    #else
      #define GRAPHICALG_EXPORT
    #endif
  #endif
#endif

GRAPHICALG_EXPORT TS_Point *GRAPHICALGCALL  TS_CalcLine(const TS_Segment */*in*/aLine, int * /*out*/ oSteps);
GRAPHICALG_EXPORT int GRAPHICALGCALL TS_CalcCircleInto(int x, int y, int r, int steps, int * /*out*/oInto, size_t stride);
GRAPHICALG_EXPORT int GRAPHICALGCALL TS_CalcCircleIntoFloat(int x, int y, int r, int steps, float * /*out*/oInto, size_t stride);
GRAPHICALG_EXPORT TS_Point *GRAPHICALGCALL TS_CalcCircle(int x, int y, int r, int steps);
GRAPHICALG_EXPORT int GRAPHICALGCALL TS_DeleteLine(TS_Point *aDelete);
GRAPHICALG_EXPORT int GRAPHICALGCALL TS_DeleteCircle(TS_Point *aDelete);

#endif
