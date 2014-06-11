#ifndef SAPPHIRE_BLENDMODE_HEAD
#define SAPPHIRE_BLENDMODE_HEAD

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

#endif
