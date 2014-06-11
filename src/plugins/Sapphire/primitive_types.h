#ifndef SAPPHIRE_PRIMITIVE_TYPES_HEAD
#define SAPPHIRE_PRIMITIVE_TYPES_HEAD

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus

extern "C" {

#endif

typedef struct {
    bool cubic;
    uint32_t color;
    unsigned int step;
    int x[4];
    int y[4];
}BezierCurve_arg;

typedef struct Complex_arg{
    int x;
    int y;
    unsigned int w;
    unsigned int h;
    unsigned int radius;
    uint32_t color[2];
}Complex_arg;

typedef struct {
    int x[2];
    int y[2];
    uint32_t color[2];
}Line_arg;

typedef struct {
    int x;
    int y;
    unsigned int hradius;
    unsigned int vradius;
    float angle;
    uint32_t color[2];
}Ellipse_arg;

typedef struct {
    int x;
    int y;
    unsigned int width;
    unsigned int height;
    uint32_t color[4];
}Rectangle_arg;

typedef struct {
    int x[3];
    int y[3];
    uint32_t color[3];
}Triangle_arg;

typedef struct {
    int x;
    int y;
    uint32_t color;
}Point_arg;

#ifdef __cplusplus

}

#endif

#endif
