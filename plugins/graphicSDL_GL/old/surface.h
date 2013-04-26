#ifndef SDL_GL_SURFACE_HEAD
#define SDL_GL_SURFACE_HEAD
#include "main.h"
#include "image.h"

EXTERN_OBJECT_TEMPLATES(Surface);

void SurfaceInit(void);
void SurfaceClose(void);

v8Function NewSurface(V8ARGS);

v8Function SurfaceBlit(V8ARGS);

v8Function SurfaceToImage(V8ARGS);
v8Function SurfaceClone(V8ARGS);
v8Function SurfaceCloneSection(V8ARGS);
v8Function SurfaceGrab(V8ARGS);

v8Function SurfaceSave(V8ARGS);

v8Function SurfaceSetAlpha(V8ARGS);
v8Function SurfaceGetClippingRectangle(V8ARGS);
v8Function SurfaceSetClippingRectangle(V8ARGS);

v8Function SurfaceRectangle(V8ARGS);

void TS_SurfaceFinalizer(V8FINALIZERARGS);

#endif
