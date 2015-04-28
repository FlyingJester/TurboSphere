#pragma once
#include "chrono.hpp"

namespace Chrono {
    bool Delay(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool GetTime(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool GetSeconds(JSContext *ctx, unsigned argc, JS::Value *vp);
}
