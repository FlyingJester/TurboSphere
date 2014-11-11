#pragma once
#include "Args.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum TS_SurfError TS_SurfaceBlitSurface(struct TS_SurfaceArgs *arg);
enum TS_SurfError TS_SurfaceTransformBlitSurface(struct TS_SurfaceArgs *arg);

inline struct TS_Point TS_SurfaceUVtoXY(SDL_Surface *aSurface, float U, float V){
  float lW,lH,lX,lY;
  lW = aSurface->w; lH = aSurface->h;
  lX = lW*U; lY = lH*V;
  struct TS_Point rP = {(int)(lX), (int)(lY)};
  return rP;
}

#ifdef __cplusplus
}
#endif // __cplusplus
