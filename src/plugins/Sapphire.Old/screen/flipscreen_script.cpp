#include "flipscreen.h"
#include "../loadfunc/thread_extra.h"
#include "../../../graphiccommon/screen.h"

    /////
    // Flipping and Frame Rate

    Turbo::JSFunction Script::FlipScreen(Turbo::JSArguments args){
        TS_FlipScreen();
    }

    Turbo::JSFunction Script::GetFrameRate(Turbo::JSArguments args){

    }

    Turbo::JSFunction Script::SetFrameRate(Turbo::JSArguments args){

    }

    /////
    // Clipping Rectangle

    Turbo::JSFunction Script::GetClippingRectangle(Turbo::JSArguments args){

    }

    Turbo::JSFunction Script::SetClippingRectangle(Turbo::JSArguments args){

    }

    Turbo::JSFunction Script::GetScreenWidth(Turbo::JSArguments args){
        args.GetReturnValue().Set(v8::Integer::New(v8::Isolate::GetCurrent(), ::GetScreenWidth()));
    }

    Turbo::JSFunction Script::GetScreenHeight(Turbo::JSArguments args){
        args.GetReturnValue().Set(v8::Integer::New(v8::Isolate::GetCurrent(), ::GetScreenHeight()));

    }
