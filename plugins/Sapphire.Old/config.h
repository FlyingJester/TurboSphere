#ifndef SAPPHIRE_CONFIG
#define SAPPHIRE_CONFIG

#ifdef __cplusplus
struct Configuration_t{
#else
typedef struct {
#endif
    int niceImages; //Bool
    int niceCircles; //Bool
    int fullscreen; //Bool
    int vsync; //Bool
    float scale; //>=0

    int composite; //Bool
    int surfaceThread; //Bool
    int glThread; //Bool

#ifdef __cplusplus
};
#else
} Configuration_t;
#endif

extern Configuration_t configl;

#endif
