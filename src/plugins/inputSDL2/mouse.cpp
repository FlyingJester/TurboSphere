#define INPUT_INTERNAL
#define PLUGINNAME "InputSDL2"
#include "main.h"

Turbo::JSFunction GetMouseX(Turbo::JSArguments args){
	SDL_PumpEvents();
	int x = 0;
	SDL_GetMouseState(&x, NULL);
	args.GetReturnValue().Set(v8::Int32::New(iso, x));
}

Turbo::JSFunction GetMouseY(Turbo::JSArguments args){
	SDL_PumpEvents();
	int	y = 0;
	SDL_GetMouseState(NULL, &y);
	#ifdef OS_X
    y-=16;
	#endif
	args.GetReturnValue().Set( v8::Int32::New(iso, y));
}

Turbo::JSFunction IsMouseButtonPressed(Turbo::JSArguments args){
    int sig[] = {Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig, true))
        return;
    //Just inline it to death.
	args.GetReturnValue().Set( v8::Boolean::New(iso, SDL_GetMouseState(NULL, NULL)&(1<<args[0]->Int32Value())));
}
