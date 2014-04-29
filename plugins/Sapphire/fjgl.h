#ifndef FJGL_API_HEAD
#define FJGL_API_HEAD

/////
// FJGL Header
//
// Contains the functions intended for direct use from without Sapphire
//

#include <GL/gl.h>

#ifdef _WIN32
#define STDCALL __stdcall
#else
#define STDCALL
#endif


#ifdef __cplusplus
#include <string>
#define TYPE(x, y) x y
#ifdef __linux__
#define EXPORT(x) extern "C" x
#endif
#ifdef _WIN32
#define EXPORT(x) extern "C" __declspec( dllexport ) x
#endif

#else
#ifndef _WIN32
#include <stdbool.h>
#else

#define EXPORT(x) __declspec( dllexport ) x

typedef char bool;
#define true 1;
#define false 0;

#define inline __inline

#endif

#define TYPE(x, y) typdef y x
#ifndef EXPORT
#define EXPORT(x) x
#endif
#endif

#ifdef __GNUC__
#define ALIGN __attribute__((aligned(16)))
#else
#define ALIGN
#endif

#include <inttypes.h>

typedef uint32_t RGBA;

#ifdef __cplusplus
struct DriverInfo_t{
#else
typedef struct {
#endif
    const char* name;
    const char* author;
    const char* date;
    const char* version;
    const char* description;
#ifdef __cplusplus
};
#else
} DriverInfo_t;
#endif


#ifdef __cplusplus
enum BlendMode{
#else
typedef enum {
#endif
    bmBlend = 0,
    bmReplace,
    bmRGB,
    bmAlpha,
    bmAdd,
    bmSubtract,
    bmMultiply,
    bmAverage,
    bmInvert,
    bmSquare,
    bmNone
#ifdef __cplusplus
};
#else
} BlendMode;
#endif

#ifdef __cplusplus
struct IMAGE{
#else
typedef struct {
#endif
    RGBA *pixels;
    GLuint texture;
    unsigned int w;
    unsigned int h;
GLuint TexCoordBuffer;
GLuint ColorBuffer;
GLuint VertexBuffer;
GLuint VertexArray; //Only valid when configl.hasVertexArrays is not zero.
#ifdef __cplusplus
};
#else
} ALIGN IMAGE;
#endif


/*
#ifdef __linux__
EXPORT(bool InternalInitVideo(int w, int h));

#else
EXPORT(void STDCALL ConfigureDriver(HWND parent));
EXPORT(bool STDCALL InitVideoDriver(HWND window, int w, int h));

#endif
*/

EXPORT(void STDCALL GetDriverInfo(DriverInfo_t* driverinfo));

EXPORT(void STDCALL CloseVideoDriver(void));
EXPORT(bool STDCALL ToggleFullScreen(void));

EXPORT(void STDCALL FlipScreen(void));
EXPORT(void STDCALL SetClippingRectangle(int x, int y, int w, int h));
EXPORT(void STDCALL GetClippingRectangle(int* x, int* y, int* w, int* h));

EXPORT(IMAGE * STDCALL CreateImage(int width, int height, RGBA* pixels));
EXPORT(IMAGE * STDCALL CloneImage(IMAGE * image));
EXPORT(IMAGE * STDCALL GrabImage(int x, int y, int width, int height));
EXPORT(void STDCALL DestroyImage(IMAGE * image));
EXPORT(void STDCALL BlitImage(IMAGE * image, int x, int y, BlendMode blendmode));
EXPORT(void STDCALL BlitImageMask(IMAGE * image, int x, int y, BlendMode blendmode, RGBA mask, BlendMode mask_blendmode));
EXPORT(void STDCALL TransformBlitImage(IMAGE * image, int *x, int *y, BlendMode blendmode));
EXPORT(void STDCALL TransformBlitImageMask(IMAGE * image, int x[4], int y[4], BlendMode blendmode, RGBA mask, BlendMode mask_blendmode));
EXPORT(int STDCALL GetImageWidth(IMAGE * image));
EXPORT(int STDCALL GetImageHeight(IMAGE * image));
EXPORT(RGBA* STDCALL LockImage(IMAGE * image));
EXPORT(void STDCALL UnlockImage(IMAGE * image));
EXPORT(void STDCALL DirectBlit(int x, int y, int w, int h, RGBA* pixels));
EXPORT(void STDCALL DirectTransformBlit(int x[4], int y[4], int w, int h, RGBA* pixels));
EXPORT(void STDCALL DirectGrab(int x, int y, int w, int h, RGBA* pixels));

EXPORT(void STDCALL DrawPoint(int x, int y, RGBA color));
EXPORT(void STDCALL DrawPointSeries(int** points, int length, RGBA color));
EXPORT(void STDCALL DrawLine(int x[2], int y[2], RGBA color));
EXPORT(void STDCALL DrawGradientLine(int x[2], int y[2], RGBA colors[2]));
EXPORT(void STDCALL DrawLineSeries(int** points, int length, RGBA color, int type));
EXPORT(void STDCALL DrawBezierCurve(int x[4], int y[4], double step, RGBA color, int cubic));
EXPORT(void STDCALL DrawTriangle(int x[3], int y[3], RGBA color));
EXPORT(void STDCALL DrawGradientTriangle(int x[3], int y[3], RGBA colors[3]));
EXPORT(void STDCALL DrawPolygon(int** points, int length, int invert, RGBA color));
EXPORT(void STDCALL DrawOutlinedRectangle(int x, int y, int w, int h, int size, RGBA color));
EXPORT(void STDCALL DrawRectangle(int x, int y, int w, int h, RGBA color));
EXPORT(void STDCALL DrawGradientRectangle(int x, int y, int w, int h, RGBA colors[4]));
EXPORT(void STDCALL DrawOutlinedComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, RGBA color, int antialias));
EXPORT(void STDCALL DrawFilledComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[2]));
EXPORT(void STDCALL DrawGradientComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[3]));
EXPORT(void STDCALL DrawOutlinedEllipse(int x, int y, int rx, int ry, RGBA color));
EXPORT(void STDCALL DrawFilledEllipse(int x, int y, int rx, int ry, RGBA color));
EXPORT(void STDCALL DrawOutlinedCircle(int x, int y, int r, RGBA color, int antialias));
EXPORT(void STDCALL DrawFilledCircle(int x, int y, int r, RGBA color, int antialias));
EXPORT(void STDCALL DrawGradientCircle(int x, int y, int r, RGBA colors[2], int antialias));

EXPORT(unsigned int GetVertexAttrib(void));
EXPORT(unsigned int GetColorAttrib(void));
EXPORT(unsigned int GetTexCoordAttrib(void));

#ifdef EXPORT
    #undef EXPORT
#endif

#endif
