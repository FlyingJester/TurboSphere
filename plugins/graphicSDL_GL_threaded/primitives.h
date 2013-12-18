#ifndef SDL_GL_PRIMITIVES_HEAD
#define SDL_GL_PRIMITIVES_HEAD
#include "main.h"
#include "../../common/dlopenwrap.h"

extern double    (*sin_override)(double);
extern double    (*cos_override)(double);
extern double    (*floor_override)(double);
extern double    (*ceil_override)(double);
extern double    (*max_override)(double, double);
extern double    (*min_override)(double, double);
extern double    (*sqrt_override)(double);
static double tempmin(double a, double b){
    return min(a, b);
}
static double tempmax(double a, double b){
    return max(a, b);
}

v8Function Rectangle(V8ARGS);
v8Function Line(V8ARGS);
v8Function Point(V8ARGS);
v8Function Triangle(V8ARGS);
v8Function Polygon(V8ARGS);

v8Function GradientRectangle(V8ARGS);
v8Function GradientLine(V8ARGS);
v8Function GradientTriangle(V8ARGS);

v8Function OutlinedRectangle(V8ARGS);
v8Function OutlinedPolygon(V8ARGS);

v8Function OutlinedCircle(V8ARGS);
v8Function FilledCircle(V8ARGS);
v8Function GradientCircle(V8ARGS);

void TS_Rectangle(int, int, int, int, TS_Color*);
void TS_Line(int x1, int y1, int x2, int y2, TS_Color *color);
void TS_Pixel(int x, int y, TS_Color *color);
void TS_Triangle(int x1, int y1, int x2, int y2, int x3, int y3, TS_Color *color);
void TS_Polygon(int numPoints, int *x, int *y, TS_Color *color);
void TS_FilledCircle(int x, int y, int r, TS_Color *c, bool AA);

void TS_GradientRectangle(int x, int y, int w, int h, TS_Color *color1, TS_Color *color2, TS_Color *color3, TS_Color *color4);
void TS_GradientLine(int x1, int y1, int x2, int y2, TS_Color *color1, TS_Color *color2);
void TS_GradientTriangle(int x1, int y1, int x2, int y2, int x3, int y3, TS_Color *color1, TS_Color *color2, TS_Color *color3);
void TS_GradientCircle(int x, int y, int r, TS_Color *c1, TS_Color *c2, bool AA);
void TS_GradientPolygon(int numPoints, int *x, int *y, TS_Color *color);

void TS_OutlinedRectangle(int x, int y, int w, int h, TS_Color *color, int thickness);
void TS_OutlinedPolygon(int numPoints, int *x, int *y, TS_Color *color);
void TS_OutlinedCircle(int x, int y, int r, TS_Color *c, bool AA);

void TS_SoftLine(void *);
void TS_SoftGradientRectangle(int x, int y, int w, int h, TS_Color *c1, TS_Color *c2, TS_Color *c3, TS_Color *c4, SDL_Surface *destination);
void TS_SoftFilledCircle(int x, int y, int rad, TS_Color *c, SDL_Surface *destination);
void TS_SoftOutlinedCircle(int x, int y, int rad, TS_Color *c, SDL_Surface* destination);
void TS_SoftRectangle(int x, int y, int w, int h, TS_Color *c, SDL_Surface *destination);

void PrimitivesInit(void);
#endif
