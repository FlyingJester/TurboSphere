#pragma once

#define PLUGINNAME "Sapphire"
#define BRACKNAME "[" PLUGINNAME "]"
#define P_WARNING BRACKNAME " Warning: "
#define P_ERROR   BRACKNAME " Error: "

#define IMG_FLAGS (IMG_INIT_JPG|IMG_INIT_PNG|IMG_INIT_TIF)

#ifdef __cplusplus

#include <TSPR/concurrent_queue.h>

#ifdef __GNUC__
  #pragma GCC diagnostic ignored "-Wnull-arithmetic"
  #pragma GCC diagnostic push
#endif // GNU

#include <pluginsdk/plugin.h>

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif //GNU

#endif //c++

#define Frmask 0x000000FF
#define Fgmask 0x0000FF00
#define Fbmask 0x00FF0000
#define Famask 0xFF000000

#define CHANNEL_MASKS Frmask, Fgmask, Fbmask, Famask
#define IMAGE_DEPTH 32

// This should ONLY be used in C and when T5 can't be used directly.
#ifdef __cplusplus
extern "C"{
#endif
int T5_StdOut_Write(const char *aToWrite);
int T5_StdErr_Write(const char *aToWrite);
#ifdef __cplusplus
}


namespace Sapphire {

template <typename A>
class DoNothing{
public:
  void operator () (A) {}
};

template <class A, class B, class C = DoNothing<A *> >
class GenericHolder {
  public:
  A *mWraps;
  B *b;
  GenericHolder(A *a){
    printf(BRACKNAME " Info: creating RAII primitive.\n");
    mWraps = a;
    b = new B();
    C(mWraps);
  }
  ~GenericHolder(){
    printf(BRACKNAME " Info: destroying RAII primitive.\n");
    (*b)(mWraps);
    printf(BRACKNAME " Info: Wrapped val is %p.\n", mWraps);
    delete b;
  }

};

}

#endif // __cplusplus
