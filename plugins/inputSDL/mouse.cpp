#define INPUT_INTERNAL
#include "main.h"

v8Function GetMouseX(V8ARGS)
{
	SDL_PumpEvents();
	int x = 0;
	SDL_GetMouseState(&x, NULL);
	return v8::Int32::New(x);
}

v8Function GetMouseY(V8ARGS)
{
	SDL_PumpEvents();
	int	y = 0;
	SDL_GetMouseState(NULL, &y);
	return v8::Int32::New(y);
}

v8Function IsMouseButtonPressed(V8ARGS)
{
	if(args.Length()<1){
        THROWERROR("IsMouseButtonPressed Error: No arguments.");
	}
    CHECK_ARG_INT(0, "IsMouseButtonPressed Error: Argument 0 is not a number.");
    //Just inline it to death.
	return v8::Boolean::New(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(args[0]->Int32Value()+1));
}
