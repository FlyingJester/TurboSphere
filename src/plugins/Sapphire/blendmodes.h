#ifndef SAPPHIRE_BLENDMODES_HEAD
#define SAPPHIRE_BLENDMODES_HEAD

#ifdef __cplusplus
enum TS_Blendmode {
#else
typedef enum {
#endif
    BLEND,
    REPLACE,
    RGB_ONLY,
    ALPHA_ONLY,
    ADD,
    SUBTRACT,
    MULTIPLY,
    AVERAGE,
    INVERT,
    SQUARE,
    NUM_BLENDMODES,
#ifdef __cplusplus
};
#else
} TS_Blendmode;
#endif

#endif
