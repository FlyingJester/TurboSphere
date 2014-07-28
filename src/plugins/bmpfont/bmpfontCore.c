#define TS_BMPFONT_INTERNAL 1

#include <stdlib.h>
#include <string.h>

struct RGBA_Pixels;

typedef struct {

    uint32_t num_chars;
    struct RGBA_Pixels *chars;

    uint64_t total_size_of_chars; // Not including headers.

} TS_bmpfont;

#include "bmpfontCore.h"

#include <assert.h>


const size_t TS_bmpSigSize = 0x04;
const size_t TS_bmpHeaderSize = 0x100;
const size_t TS_bmpCharHeaderSize = 0x20;

typedef union {uint16_t w[1]; uint8_t b[2];} MemWord;

TS_bmpError TS_BMPInit(){
  return bmpSuccess;
}

TS_bmpError TS_BMPClose(){
  return bmpSuccess;
}

TS_bmpError TS_BMPFontReadHeader(TS_bmpfont *aFont, const uint8_t *aHeader,
                                 TS_bmpType *oType){

    char lSig[TS_bmpSigSize];
    MemWord lVersion;
    MemWord lNumChars1;
    MemWord lNumChars2;

    lVersion.w[0]  = 0;

    lNumChars1.w[0] = 0;
    lNumChars2.w[0] = 0;

    assert(aFont);
    assert(aHeader);

    *oType = 0;

    memcpy(lSig, aHeader, TS_bmpSigSize);

    if(memcmp(lSig, ".rfn", TS_bmpSigSize))
        return bmpBadSig;


    lVersion.b[0] = aHeader[4];
    lVersion.b[1] = aHeader[5];

    if((lVersion.w[0]==0) || (lVersion.w[0] > 4))
        return bmpBadType;

    *oType = lVersion.w[0];

    lNumChars1.b[0] = aHeader[6];
    lNumChars1.b[1] = aHeader[7];
    lNumChars2.b[0] = aHeader[8];
    lNumChars2.b[1] = aHeader[9];

    aFont->num_chars  = lNumChars1.w[0];
    aFont->num_chars += lNumChars2.w[0]<<16;

    return bmpSuccess;

}

TS_bmpfont *TS_BMPCreateEmptyFont(){

    TS_bmpfont *rFont = malloc(sizeof(TS_bmpfont));
    assert(rFont);

    rFont->num_chars = 0;
    rFont->chars = NULL;
    rFont->total_size_of_chars = 0;

    return rFont;
}

TS_bmpError TS_BMPReadPixels(struct RGBA_Pixels *aTo, TS_bmpType aType);

TS_bmpError TS_BMPReadPixelsCFile(struct RGBA_Pixels *aTo, FILE *aFrom, TS_bmpType aType){

    int r1 = 0;

    aTo->pixels = malloc( aTo->w * aTo->h * 4 );
    assert(aTo->pixels!=NULL);

    switch(aType){
        case TS_bmpUnknown: return bmpBadType;
        case TS_bmp1BPPalphascale:
        case TS_bmpRGBA1BPP:
        case TS_bmp1BPP:
        case TS_bmp1BPPpacked:
          r1 = fread(aTo->pixels +
                     ( aTo->w * aTo->h * 3),
                     1, aTo->w * aTo->h, aFrom);
          break;
        case TS_bmpRGBA:
          r1 = fread(aTo->pixels,
                     1, aTo->w * aTo->h * 4, aFrom);
          break;
        default:
          r1 = 0;
    }

    assert(r1!=0);

    return TS_BMPReadPixels(aTo, aType);

}

TS_bmpError TS_BMPReadPixelsMem(struct RGBA_Pixels *aTo, const void *aFrom, TS_bmpType aType){

    int r1 = 0;

    aTo->pixels = malloc( aTo->w * aTo->h * 4 );
    assert(aTo->pixels!=NULL);

    switch(aType){
        case TS_bmpUnknown: return bmpBadType;
        case TS_bmp1BPPalphascale:
        case TS_bmpRGBA1BPP:
        case TS_bmp1BPP:
        case TS_bmp1BPPpacked:
          memcpy(aTo->pixels + ( aTo->w * aTo->h * 3), aFrom, aTo->w * aTo->h);
          r1 = 1;
          break;
        case TS_bmpRGBA:
          memcpy(aTo->pixels, aFrom, aTo->w * aTo->h * 4);
          r1 = 1;
          break;
        default:
          r1 = 0;
    }

    assert(r1!=0);

    return TS_BMPReadPixels(aTo, aType);

}

TS_bmpError TS_BMPReadPixels(struct RGBA_Pixels *aTo, TS_bmpType aType){

    int r1 = 0;

    switch(aType){
        case TS_bmpUnknown: return bmpBadType;
        case TS_bmpRGBA: // Real version 2.
          r1 = 1;
          break;
        case TS_bmp1BPPalphascale: // Real version 1.
          while(aTo->w * aTo->h){

              uint32_t pixel = 0x00FFFFFF | ( ((uint8_t *)(aTo->pixels))
                [ ( aTo->w * aTo->h * 3) + r1 ] << 24);

              aTo->pixels[r1] = pixel;

              r1++;

          }
          break;
        case TS_bmpRGBA1BPP: // Ech byte is 0bRRGGBBAA
          while(r1<aTo->w * aTo->h){

              uint8_t pixel8 = ( ((uint8_t *)(aTo->pixels))
                [ ( aTo->w * aTo->h * 3) + r1 ]);

              uint8_t rp = ((pixel8 >> 0 ) & 0x3) * 0x3F;
              uint8_t gp = ((pixel8 >> 2 ) & 0x3) * 0x3F;
              uint8_t bp = ((pixel8 >> 4 ) & 0x3) * 0x3F;
              uint8_t ap = ((pixel8 >> 6 ) & 0x3) * 0x3F;

              aTo->pixels[r1] |= ap & 0xFF;
              aTo->pixels[r1] <<= 8;
              aTo->pixels[r1] |= bp & 0xFF;
              aTo->pixels[r1] <<= 8;
              aTo->pixels[r1] |= gp & 0xFF;
              aTo->pixels[r1] <<= 8;
              aTo->pixels[r1] |= rp & 0xFF;

              r1++;
          }
          break;
        case TS_bmp1BPP:
          while(r1<aTo->w * aTo->h){
              if(( ((uint8_t *)(aTo->pixels))
                    [ ( aTo->w * aTo->h * 3) + r1 ])==0)
                  aTo->pixels[r1] = 0xFF000000;
              else if (( ((uint8_t *)(aTo->pixels))
                    [ ( aTo->w * aTo->h * 3) + r1 ])==0xFF)
                  aTo->pixels[r1] = 0xFFFFFFFF;
              else
                  aTo->pixels[r1] = 0x00000000;

              r1++;
          }
          break;
        default:
          r1 = 0;
    }

    assert(r1!=0);

    return bmpSuccess;

}

void TS_BMPReadCharCFile(TS_bmpfont *aTo, FILE *aFrom, size_t at, size_t to,
                    TS_bmpType aType){

    if(at >= to)
      return;

    int r1 = 0;

    if(feof(aFrom) || ferror(aFrom)){
      fprintf(stderr, "[bmpfont] Core Error: File handle died unexpectedly.\n");
      return;
    }

    assert(aType!=0);
    assert(aType<=4);

    MemWord lSize;// = 0;
    r1 = fread(lSize.w, 1, 2, aFrom);
    aTo->chars[at].w = lSize.w[0];
    assert(r1==2);

    r1 = fread(lSize.w, 1, 2, aFrom);
    aTo->chars[at].h = lSize.w[0];
    assert(r1==2);

    if((aTo->chars[at].w == 0 ) || (aTo->chars[at].w > 64))
      printf("[bmpfont] Core info: Found a suspicious width of %i.\n", aTo->chars[at].w);
    //else
    //  printf("[bmpfont] Core info: Recording width of %i.\n", aTo->chars[at].w);

    fseek(aFrom, 28, SEEK_CUR);

    if((aTo->chars[at].w*aTo->chars[at].h)>0){
      //fprintf(stderr, "[bmpfont] Core Info: Reading.\n");
        TS_BMPReadPixelsCFile(&(aTo->chars[at]), aFrom, aType);

        aTo->total_size_of_chars += aTo->chars[at].w * aTo->chars[at].h * 4;
    }

    TS_BMPReadCharCFile(aTo, aFrom, ++at, to, aType);

}

void TS_BMPReadCharMem(TS_bmpfont *aTo, const uint8_t *aFrom, size_t at, size_t to,
                       TS_bmpType aType){
    assert(aType!=0);
    assert(aType<=4);

    aTo->chars[at].w = aFrom[0] | ( aFrom[1] << 8 );
    aTo->chars[at].h = aFrom[2] | ( aFrom[3] << 8 );

    if((aTo->chars[at].w*aTo->chars[at].h)>0){
        TS_BMPReadPixelsMem(&(aTo->chars[at]), aFrom, aType);

        aTo->total_size_of_chars += aTo->chars[at].w * aTo->chars[at].h * 4;
    }

    aFrom += TS_bmpCharHeaderSize + (aTo->chars[at].w * aTo->chars[at].h * 4);

    if(at < to)
      TS_BMPReadCharMem(aTo, aFrom, ++at, to, aType);
}

TS_bmpfont *TS_BMPOpenFontCFile(FILE *aFrom){

    if(feof(aFrom) || ferror(aFrom)){
      fprintf(stderr, "[bmpfont] Core Error: Invalid file handle.\n");
      return NULL;
    }

    const size_t ts = ftell(aFrom);
    fseek(aFrom, 0, SEEK_END);

    const size_t s = ftell(aFrom);
    fseek(aFrom, ts, SEEK_SET);

    if(s < TS_bmpHeaderSize){
      fprintf(stderr, "[bmpfont] Core Error: File too small to contain a font.\n");
      return NULL;
    }

    uint8_t HeaderBuffer[TS_bmpHeaderSize];

    const size_t r1 = fread(HeaderBuffer, 1, TS_bmpHeaderSize, aFrom);

    assert(r1==TS_bmpHeaderSize);

    TS_bmpfont *rFont = malloc(sizeof(TS_bmpfont));
    assert(rFont);

    TS_bmpType lType = 0;
    int err = TS_BMPFontReadHeader(rFont, HeaderBuffer, &lType);

    if(err){
        fprintf(stderr, "[bmpfont] Core Error: ");
        switch(err){
            case bmpBadType:
            fprintf(stderr, "Bad header version.\n");
            break;
            case bmpBadSig:
            fprintf(stderr, "Bad header signature.\n");
            break;

        }
        free(rFont);
        return NULL;
    }

    fprintf(stderr, "[bmpfont] Core Info: %i chars.\n", rFont->num_chars);

    rFont->chars = malloc(sizeof(struct RGBA_Pixels ) * rFont->num_chars);
    TS_BMPReadCharCFile(rFont, aFrom, 0, rFont->num_chars, lType);

    return rFont;
}

TS_bmpfont *TS_BMPOpenFontFromMem(const void *aFrom1, ptrdiff_t aSize){
    if(aSize < 0xFF)
      return NULL;

    const uint8_t *aFrom = aFrom1;

    TS_bmpfont *rFont = malloc(sizeof(TS_bmpfont));
    assert(rFont);

    TS_bmpType lType = 0;

    int err = TS_BMPFontReadHeader(rFont, aFrom1, &lType);

    if(err){
        free(rFont);
        return NULL;
    }

    rFont->chars = malloc(sizeof( struct RGBA_Pixels) * rFont->num_chars);
    TS_BMPReadCharMem(rFont, &(aFrom[TS_bmpHeaderSize]), 0, rFont->num_chars, lType);

    return rFont;
}

TS_bmpError TS_DestroyFont(TS_bmpfont *aDestroy){

    for(int i = 0; i < aDestroy->num_chars; i++){
        free(aDestroy->chars[i].pixels);
    }

    free(aDestroy->chars);
    free(aDestroy);

    return bmpSuccess;
}

// Gets the size that the font will take up when saved.
size_t TS_GetFontFileSize(TS_bmpfont *aFrom){
    return aFrom->total_size_of_chars
      + (aFrom->num_chars * TS_bmpCharHeaderSize)
      + TS_bmpHeaderSize;
}

TS_bmpError TS_SaveFontToMem(TS_bmpfont *aFrom, void *aTo1){
    uint8_t  *aTo  = aTo1;
    uint16_t *aToS = aTo1;

    memset(aTo1, 0, TS_bmpHeaderSize);

    memcpy(aTo, ".rfn", 4);

    aToS[2] = 2;

    uint32_t lNumChars = aFrom->num_chars;
    memcpy(aTo1 + 6, &lNumChars, 4);

    aTo+=TS_bmpHeaderSize;

    for(int i = 0; i < aFrom->num_chars; i++){
        uint16_t w = aFrom->chars[i].w;
        uint16_t h = aFrom->chars[i].h;
        const size_t lS = w*h<<2;
        memcpy(aTo+0, &w, 2);
        memcpy(aTo+2, &h, 2);
        aTo+=TS_bmpCharHeaderSize;
        memcpy(aTo, aFrom->chars[i].pixels, lS);
        aTo += lS;
    }

    return bmpSuccess;

}

TS_bmpError TS_SaveFontToCFile(TS_bmpfont *aFrom, FILE *aTo){

    fprintf(aTo, ".rfn");

    uint16_t lVersion = 2;
    uint32_t lNumChars = aFrom->num_chars;

    fwrite(&lVersion,  2, 1, aTo);
    fwrite(&lNumChars, 2, 2, aTo);

    for(int e = 0; e < 248; e++)
      fputc(0, aTo);

    for(int i = 0; i < aFrom->num_chars; i++){
        uint16_t w = aFrom->chars[i].w;
        uint16_t h = aFrom->chars[i].h;
        const size_t lS = w*h<<2;
        fwrite(&w, 2, 1, aTo);
        fwrite(&h, 2, 1, aTo);
        for(int e = 0; e < 28; e++)
          fputc(0, aTo);

        fwrite(aFrom->chars[i].pixels, 1, lS, aTo);

    }

    return bmpSuccess;

}

// Adds a 32-bit RGBA character to the font.
TS_bmpError TS_BMPAddCharacter(TS_bmpfont *aFont, uint32_t aCodePoint,
                               struct RGBA_Pixels *aChar);


// 1 bit packed image.
TS_bmpError TS_BMPAddPackedCharacter(TS_bmpfont *aFont, uint32_t aCodePoint,
                                     struct RGBA_Pixels *aChar);

struct RGBA_Pixels TS_BMPGetCharacter(TS_bmpfont *aFont, uint32_t aCodePoint){
    return aFont->chars[aCodePoint];
};

TS_bmpError TS_BMPGetUTF8Character(TS_bmpfont *aFont, struct RGBA_Pixels* aTo,
                                   const char *aCodePoint, size_t *aBytes){

    uint8_t lFirst = *aCodePoint;
    *aBytes = 1;
    if((lFirst&(0x80))==0)
      return TS_BMPGetASCIICharacter(aFont, aTo, *aCodePoint);

    uint64_t lCodePoint = 0;

    size_t lSize = 0;

    while((lFirst&(0x80))==0x80){
      lFirst <<= 1;
      lSize++;
    }

    *aBytes = lSize;
    lCodePoint = lFirst >> (lSize+1);
    lCodePoint <<= 6*(lSize-1);

    for(int i = 1; i < lSize; i++){
        if(((aCodePoint[i]&0x80)!=0x80)||((aCodePoint[i]&0x40)==0x40))
          return bmpBadUtf;
        lCodePoint+= (aCodePoint[i]&0x3F) << 6*(lSize-i);
    }

    assert(lCodePoint < aFont->num_chars);

    aTo->w = aFont->chars[lCodePoint].w;
    aTo->h = aFont->chars[lCodePoint].h;
    aTo->pixels = aFont->chars[lCodePoint].pixels;
    // = TS_BMPGetCharacter(aFont, lCodePoint);
    return bmpSuccess;

}

TS_bmpError TS_BMPGetASCIICharacter(TS_bmpfont *aFont, struct RGBA_Pixels* aTo,
                                    uint8_t aCodePoint){
    struct RGBA_Pixels lpixels =  TS_BMPGetCharacter(aFont, aCodePoint);
    *aTo = lpixels;
    return bmpSuccess;
}

TS_bmpError TS_BMPGetUTF8String(TS_bmpfont *aFont, struct RGBA_string* aTo,
                                const char * aString){

    size_t length = 0;
    size_t b = 0;
    aTo->pixelarray = NULL;
    while(*aString!=0){

       // printf("[bmpfont] Core Info: examining character %c (%x)\n", *aString, *aString);

        length++;

        struct RGBA_Pixels lPixels;

        int err = TS_BMPGetUTF8Character(aFont, &lPixels, aString, &b);
        if(err!=bmpSuccess)
          return err;


        aString += b;

        aTo->pixelarray = realloc(aTo->pixelarray, length*sizeof(struct RGBA_Pixels));
        aTo->pixelarray[length-1] = lPixels;
       // printf("[bmpfont] Core info: character has a width of %i.\n", lPixels.w);
    }

    aTo->length = length;

    return bmpSuccess;

}

TS_bmpError TS_BMPGetASCIIString(TS_bmpfont *aFont, struct RGBA_string* aTo,
                                 const char * aString){
    aTo->length = strlen(aString);
    aTo->pixelarray = malloc(aTo->length*sizeof(struct RGBA_Pixels));
    for(int i = 0; i<aTo->length; i++){
        int err = TS_BMPGetASCIICharacter(aFont, &(aTo->pixelarray[i]),
                                           aString[i]);
        if(err!=bmpSuccess)
          return err;
    }

    return bmpSuccess;
}


