#pragma once
#include <stdio.h>
#include <stddef.h>

#ifndef TS_BMPFONT_INTERNAL
typedef void TS_bmpfont;
#endif

typedef enum TS_bmpError{
  bmpSuccess,
  bmpFailure, //General failure.
  bmpBadUtf, //UTF8 was invalid.
  bmpBadSig, //UTF8 was invalid.
  bmpBadType,
} TS_bmpError;

typedef enum TS_bmpType {
  TS_bmpUnknown,
  TS_bmp1BPPalphascale,
  TS_bmpRGBA,
  TS_bmpRGBA1BPP,
  TS_bmp1BPP,
  TS_bmp1BPPpacked,
} TS_bmpType;

struct RGBA_Pixels {
  uint32_t *pixels;
  unsigned short w, h;
};

struct RGBA_string {
  struct RGBA_Pixels *pixelarray;
  unsigned length;
};

#ifdef __cplusplus
extern "C" {
#endif

TS_bmpError TS_BMPInit();
TS_bmpError TS_BMPClose();

TS_bmpfont *TS_BMPCreateEmptyFont();

TS_bmpfont *TS_BMPOpenFontCFile(FILE *aFrom);

TS_bmpfont *TS_BMPOpenFontFromMem(const void *aFrom, ptrdiff_t aSize);

TS_bmpError TS_DestroyFont(TS_bmpfont *aDestroy);

// Gets the size that the font will take up when saved.
size_t TS_GetFontFileSize(TS_bmpfont *aFrom);
TS_bmpError TS_SaveFontToMem(TS_bmpfont *aFrom, void *aTo);
TS_bmpError TS_SaveFontToCFile(TS_bmpfont *aFrom, FILE *aTo);

// Adds a 32-bit RGBA character to the font.
TS_bmpError TS_BMPAddCharacter(TS_bmpfont *aFont, uint32_t aCodePoint,
                               struct RGBA_Pixels *aChar);

// 1 bit packed image.
TS_bmpError TS_BMPAddPackedCharacter(TS_bmpfont *aFont, uint32_t aCodePoint,
                                     struct RGBA_Pixels *aChar);

struct RGBA_Pixels TS_BMPGetCharacter(TS_bmpfont *aFont, uint32_t aCodePoint);

TS_bmpError TS_BMPGetUTF8Character(TS_bmpfont *aFont, struct RGBA_Pixels* aTo,
                                   const char *aCodePoint, size_t *aSize);
TS_bmpError TS_BMPGetASCIICharacter(TS_bmpfont *aFont, struct RGBA_Pixels* aTo,
                                    uint8_t aCodePoint);

TS_bmpError TS_BMPGetUTF8String(TS_bmpfont *aFont, struct RGBA_string* aTo,
                                const char * aString);

TS_bmpError TS_BMPGetASCIIString(TS_bmpfont *aFont, struct RGBA_string* aTo,
                                 const char * aString);



#ifdef __cplusplus
}
#endif
