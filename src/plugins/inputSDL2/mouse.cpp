#include "input_main.hpp"
#include <SDL2/SDL_events.h>

namespace InputSDL2{
namespace Script{

bool GetMouseX(JSContext *ctx, unsigned argc, JS::Value *vp){
	SDL_PumpEvents();
	int x = 0;
	SDL_GetMouseState(&x, nullptr);
	CallArgsFromVp(argc, vp).rval().set(JS_NumberValue(x));
    return true;
}

bool GetMouseY(JSContext *ctx, unsigned argc, JS::Value *vp){
	SDL_PumpEvents();
	int	y = 0;
	SDL_GetMouseState(nullptr, &y);
	CallArgsFromVp(argc, vp).rval().set(JS_NumberValue(y));
    return true;
}

bool IsMouseButtonPressed(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckForSingleArg(ctx, args, Turbo::Number, __func__))
        return false;
        
    //Just inline it to death.
    args.rval().set(BOOLEAN_TO_JSVAL(SDL_GetMouseState(nullptr, nullptr)&(1<<static_cast<unsigned>(args[0].toNumber()))));
    return true;
}

}
}
