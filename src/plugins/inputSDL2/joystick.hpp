#pragma once

#define REENTRANT_JOYSTICKS 0
#include "input_main.hpp"

namespace InputSDL2{
    
    void InitJoystick(JSContext *ctx);
    void CloseJoystick(JSContext *ctx);

    namespace Script{
        bool GetNumJoysticks(JSContext *ctx, unsigned argc, JS::Value *vp);
        bool GetJoystickName(JSContext *ctx, unsigned argc, JS::Value *vp);
        bool GetNumJoystickButtons(JSContext *ctx, unsigned argc, JS::Value *vp);
        bool GetNumJoystickAxes(JSContext *ctx, unsigned argc, JS::Value *vp);
        bool IsJoystickButtonPressed(JSContext *ctx, unsigned argc, JS::Value *vp);
        bool GetJoystickAxis(JSContext *ctx, unsigned argc, JS::Value *vp);
    }
}
