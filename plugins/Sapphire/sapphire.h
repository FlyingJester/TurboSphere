#ifndef PLUGINNAME
#define PLUGINNAME "Sapphire"

/////
// The Sapphire Graphics Plugin
//
//   A threaded graphics plugin for the TurboSphere game engine. It includes
// the same bindings as a Sphere graphics plugin as well, provided solely for
// use by other plugins. It cannot directly be used by Sphere 1.x, as it does
// not initilize properly for it.
//
//   Sapphire has two threaded renderers, one for software graphics (surfaces)
// and one of hardware graphics (images, primitives, the Galileo API). The
// software renderer supports out-of-order renderer processes.
//
//   The hardware renderer uses full OpenGL 4.3 and contemporary GLSL shaders.
// It supports per-operation shading and only uses full Vertex Arrays.
//

#include <stdint.h>

    #ifdef __cplusplus
    #include "../common/plugin.h"
    #endif

    #ifdef _WIN32
        #define SAPPHIRE_EXPORT __declspec(dllexport)

        #define CCALL __cdecl

    #endif
    #ifndef _WIN32
    #define CCALL
    #ifdef __cplusplus
    #define SAPPHIRE_EXPORT extern "C"
    #else
    #define SAPPHIRE_EXPORT
    #endif
    #endif

    #define NUMFUNCS 0
    #define NUMVARS  0

    #ifdef __cplusplus
    extern v8::Isolate *iso;

        #ifdef _WIN32
            extern "C" {
        #endif

        SAPPHIRE_EXPORT void                   CCALL Close(void);
        SAPPHIRE_EXPORT const char *           CCALL Init(uint32_t);
        SAPPHIRE_EXPORT int                    CCALL GetNumFunctions(void);
        SAPPHIRE_EXPORT Turbo::JSFunctionArray CCALL GetFunctions(void);
        SAPPHIRE_EXPORT Turbo::JSNameArray     CCALL GetFunctionNames(void);
        SAPPHIRE_EXPORT int                    CCALL GetNumVariables(void);
        SAPPHIRE_EXPORT Turbo::JSValueArray    CCALL GetVariables(void);
        SAPPHIRE_EXPORT Turbo::JSNameArray     CCALL GetVariableNames(void);

        #ifdef _WIN32
            }
        #endif
    #else

    #endif

#endif
