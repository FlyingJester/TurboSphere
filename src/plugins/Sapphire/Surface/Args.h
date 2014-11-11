#pragma once

#include <SDL2/SDL.h>
#include <structures.h>
#include <color.h>

enum TS_SurfError{
  tssSuccess,
  tssFailure      = 0x01,
  tssInvalidArg   = 0x02,
  tssInvalidSurf  = 0x04,
};

enum TS_BlendMode {
  tsbNone, //Default to whatever the surface has set.
  tsbBlend,
  tsbReplace,
  tsbRGB_Blend, // Preserves destination Alpha.
  tsbAlpha_Blend,
  tsbMultiply,
  tsbAdd,
  tsbSubtract,
  tsbInvert,

};

struct TS_SurfaceArgs{

  SDL_Surface *mTo;

  short x, y;
  unsigned short w, h;
  short ax,ay;
  unsigned short aw,ah;

  float f1, f2;

  TS_Color mColor;

  union {
    struct{
      SDL_Surface *mFrom;
      enum TS_BlendMode mBlendMode;
      bool mScale;
    } s;
    TS_Color extra_colors[3];
  } a;
};
