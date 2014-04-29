#ifndef SAPPHIRE_UNCLE_TUPELO_HEAD
#define SAPPHIRE_UNCLE_TUPELO_HEAD

/////
// Primitives header
//
// Defines Hard-Coded and optimized primitives.
//
// All Sphere 1.6 primitives are formed of some combination of these
//   primitives and a certain amount of JS.
//
// The functions in the FJ-GL header are only to be called directly
//   synchronously with scripts.
//
// These are immediate calls. They must be called from the drawing thread.
//

#include <stdint.h>
#include <inttypes.h>
#include "primitive_types.h"

#ifndef __cplusplus

#include <stdbool.h>

#endif

#ifdef __cplusplus

extern "C" {

#endif

void InitPrimitives(uint32_t ID);


void TS_BezierCurve(int *x, int *y, bool cubic, unsigned int step, unsigned int color);

void TS_BezierCurve_arg(BezierCurve_arg *arg);

void TS_GradientComplex(int x, int y, unsigned int w, unsigned int h, unsigned int radius, unsigned int color[2]);
void TS_GradientComplex_arg(Complex_arg *arg);

void TS_GradientLine(int x[2], int y[2], unsigned int color[2]);
void TS_GradientLine_arg(Line_arg *arg);

void TS_OutlinedEllipse(int x, int y, unsigned int hradius, unsigned int vradius, float angle, unsigned int color);
void TS_OutlinedEllipse_arg(Ellipse_arg *arg);

void TS_GradientEllipse(int x, int y, unsigned int hradius, unsigned int vradius, float angle, unsigned int color[2]);
void TS_GradientEllipse_arg(Ellipse_arg *arg);

void TS_GradientRectangle(int x, int y, unsigned int w, unsigned int h, unsigned int color[4]);
void TS_GradientRectangle_arg(Rectangle_arg *arg);

void TS_GradientTriangle(int x[3], int y[3], unsigned int color[3]);
void TS_GradientTriangle_arg(Triangle_arg *arg);

void TS_Point(int x, int y, unsigned int color);
void TS_Point_arg(Point_arg *arg);

#ifdef __cplusplus

}

#endif

#endif
