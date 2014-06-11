#ifndef SAPPHIRE_LIBRARY_API_HEAD
#define SAPPHIRE_LIBRARY_API_HEAD

#ifndef __cplusplus
    #include <stdbool.h>
#endif
#include "frontend.h"
#include "blendmode.h"
#include "image/image.h"

SAPPHIRE_EXPORT void CCALL GetClippingRectangle(int* x, int* y, int* w, int* h);
SAPPHIRE_EXPORT void CCALL GetClippingRectangle(int* x, int* y, int* w, int* h);

bool InternalInitVideo(int w, int h);

#else
SAPPHIRE_EXPORT void CCALL ConfigureDriver(HWND parent));
SAPPHIRE_EXPORT bool CCALL InitVideoDriver(HWND window, int w, int h));

#endif


SAPPHIRE_EXPORT void CCALL CloseVideoDriver(void);
SAPPHIRE_EXPORT bool CCALL ToggleFullScreen(void);

SAPPHIRE_EXPORT void CCALL FlipScreen(void);
SAPPHIRE_EXPORT void CCALL SetClippingRectangle(int x, int y, int w, int h);
SAPPHIRE_EXPORT void CCALL GetClippingRectangle(int* x, int* y, int* w, int* h);

SAPPHIRE_EXPORT TS_Image * CCALL CreateImage(int width, int height, RGBA* pixels);
SAPPHIRE_EXPORT TS_Image * CCALL CloneImage(TS_Image * Image);
SAPPHIRE_EXPORT TS_Image * CCALL GrabImage(int x, int y, int width, int height);
SAPPHIRE_EXPORT void CCALL DestroyImage(TS_Image * Image);
SAPPHIRE_EXPORT void CCALL BlitImage(TS_Image * Image, int x, int y, BlendMode blendmode);
SAPPHIRE_EXPORT void CCALL BlitImageMask(TS_Image * Image, int x, int y, BlendMode blendmode, RGBA mask, BlendMode mask_blendmode);
SAPPHIRE_EXPORT void CCALL TransformBlitImage(TS_Image * Image, int *x, int *y, BlendMode blendmode);
SAPPHIRE_EXPORT void CCALL TransformBlitImageMask(TS_Image * Image, int x[4], int y[4], BlendMode blendmode, RGBA mask, BlendMode mask_blendmode);
SAPPHIRE_EXPORT int CCALL GetImageWidth(TS_Image * Image);
SAPPHIRE_EXPORT int CCALL GetImageHeight(TS_Image * Image);
SAPPHIRE_EXPORT RGBA* CCALL LockImage(TS_Image * Image);
SAPPHIRE_EXPORT void CCALL UnlockImage(TS_Image * Image);
SAPPHIRE_EXPORT void CCALL DirectBlit(int x, int y, int w, int h, RGBA* pixels);
SAPPHIRE_EXPORT void CCALL DirectTransformBlit(int x[4], int y[4], int w, int h, RGBA* pixels);
SAPPHIRE_EXPORT void CCALL DirectGrab(int x, int y, int w, int h, RGBA* pixels);

SAPPHIRE_EXPORT void CCALL DrawPoint(int x, int y, RGBA color);
SAPPHIRE_EXPORT void CCALL DrawPointSeries(int** points, int length, RGBA color);
SAPPHIRE_EXPORT void CCALL DrawLine(int x[2], int y[2], RGBA color);
SAPPHIRE_EXPORT void CCALL DrawGradientLine(int x[2], int y[2], RGBA colors[2]);
SAPPHIRE_EXPORT void CCALL DrawLineSeries(int** points, int length, RGBA color, int type);
SAPPHIRE_EXPORT void CCALL DrawBezierCurve(int x[4], int y[4], double step, RGBA color, int cubic);
SAPPHIRE_EXPORT void CCALL DrawTriangle(int x[3], int y[3], RGBA color);
SAPPHIRE_EXPORT void CCALL DrawGradientTriangle(int x[3], int y[3], RGBA colors[3]);
SAPPHIRE_EXPORT void CCALL DrawPolygon(int** points, int length, int invert, RGBA color);
SAPPHIRE_EXPORT void CCALL DrawOutlinedRectangle(int x, int y, int w, int h, int size, RGBA color);
SAPPHIRE_EXPORT void CCALL DrawRectangle(int x, int y, int w, int h, RGBA color);
SAPPHIRE_EXPORT void CCALL DrawGradientRectangle(int x, int y, int w, int h, RGBA colors[4]);
SAPPHIRE_EXPORT void CCALL DrawOutlinedComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, RGBA color, int antialias);
SAPPHIRE_EXPORT void CCALL DrawFilledComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[2]);
SAPPHIRE_EXPORT void CCALL DrawGradientComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[3]);
SAPPHIRE_EXPORT void CCALL DrawOutlinedEllipse(int x, int y, int rx, int ry, RGBA color);
SAPPHIRE_EXPORT void CCALL DrawFilledEllipse(int x, int y, int rx, int ry, RGBA color);
SAPPHIRE_EXPORT void CCALL DrawOutlinedCircle(int x, int y, int r, RGBA color, int antialias);
SAPPHIRE_EXPORT void CCALL DrawFilledCircle(int x, int y, int r, RGBA color, int antialias);
SAPPHIRE_EXPORT void CCALL DrawGradientCircle(int x, int y, int r, RGBA colors[2], int antialias);
