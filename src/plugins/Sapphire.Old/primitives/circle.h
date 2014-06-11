/////
// Appropriated from FJ-GL.
#ifndef SAPPHIRE_FJGL_CIRCLE_HEAD
#define SAPPHIRE_FJGL_CIRCLE_HEAD

#include <math.h>
#include <stdlib.h>


/////
// Some math.h headers have this, some don't.

#ifdef M_PI
static float PI2 = M_PI*2;
#else
# define M_PI 3.14159265358979323846f //Stolen from SDL2.
static float PI2 = 6.28318530717958647692f;
#endif
static float PIB2 = M_PI/2.0f;

/////
// Array of {x, y} coordinates
#ifndef UNMANGLED
#ifdef __cplusplus
    #define UNMANGLED extern "C"
#else
    #define UNMANGLED
#endif
#endif

UNMANGLED int **ApproximateCircle(const int x, const int y, const unsigned int r, const int step) ;
UNMANGLED int *ApproximateCircleGL(const int x, const int y, const unsigned int r, const int step);
UNMANGLED int **ApproximateEllipse(const int x, const int y, const unsigned int rx,const unsigned int ry, const int step) ;
UNMANGLED int *ApproximateEllipseGL(const int x, const int y, const unsigned int rx,const unsigned int ry, const int step);

UNMANGLED void FreeCirclePoints(int **p, int step);

#define FreeCirclePointsGL(p) free(p)

#define FreeEllipsePoints FreeCirclePoints
#define FreeEllipsePointsGL FreeCirclePointsGL

#endif
