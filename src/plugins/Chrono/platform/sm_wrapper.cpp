#include "sm_wrapper.h"
#include <jsapi.h>

void TS_Chrono_GC(void *ctx){
    JS_MaybeGC(static_cast<JSContext *>(ctx));
}
