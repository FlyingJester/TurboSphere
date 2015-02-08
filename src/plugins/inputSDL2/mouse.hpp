#pragma once

#include "input_main.hpp"

namespace InputSDL2{
    namespace Script{
        bool GetMouseX(JSContext *ctx, unsigned argc, JS::Value *vp);
        bool GetMouseY(JSContext *ctx, unsigned argc, JS::Value *vp);
        bool IsMouseButtonPressed(JSContext *ctx, unsigned argc, JS::Value *vp);
    }
}
