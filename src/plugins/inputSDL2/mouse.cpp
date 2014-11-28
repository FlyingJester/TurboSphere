#define INPUT_INTERNAL
#define PLUGINNAME "InputSDL2"
#include "main.h"

Turbo::JSFunction GetMouseX(Turbo::JSArguments args){
	SDL_PumpEvents();
	int x = 0;
	SDL_GetMouseState(&x, nullptr);
	args.GetReturnValue().Set(x);
}

Turbo::JSFunction GetMouseY(Turbo::JSArguments args){
	SDL_PumpEvents();
	int	y = 0;
	SDL_GetMouseState(nullptr, &y);
	args.GetReturnValue().Set(y);
}

Turbo::JSFunction IsMouseButtonPressed(Turbo::JSArguments args){
    int sig[] = {Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig, true))
        return;
    //Just inline it to death.
	args.GetReturnValue().Set((bool)(SDL_GetMouseState(nullptr, nullptr)&(1<<args[0]->Int32Value())));
}
