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

#define Frmask 0x000000ff
#define Fgmask 0x0000ff00
#define Fbmask 0x00ff0000
#define Famask 0xff000000

#define CHANNEL_MASKS Frmask, Fgmask, Fbmask, Famask
#define IMAGE_DEPTH 32

#ifdef __cplusplus

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
  GenericHolder(A *a){
    mWraps = a;
    C(mWraps);
  }
  ~GenericHolder(){
    B(mWraps);
  }

};

}

#endif // __cplusplus
