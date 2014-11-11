#ifndef GRAPHICALG_STUB_HEAD
#define GRAPHICALG_STUB_HEAD

#define ALG_PI  3.14159265358979323846f
#define ALG_PI2 6.28318530717958647692f

#define DIFF(A, B) ((A>B)?\
          (A-B) :\
          (B-A) )

#ifndef GRAPHICALG_EXPORT
  #ifdef _WIN32
    #ifdef GRAPHICALG_INTERNAL
      #define GRAPHICALG_EXPORT __declspec(dllexport)
    #else
      #define GRAPHICALG_EXPORT __declspec(dllimport)
    #endif
  #define GRAPHICALGCALL _cdecl
  #else
    #define GRAPHICALGCALL
    #ifdef __cplusplus
      #define GRAPHICALG_EXPORT extern "C"
    #else
      #define GRAPHICALG_EXPORT
    #endif
  #endif
#endif

#endif
