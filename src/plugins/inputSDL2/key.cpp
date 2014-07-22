#define INPUT_INTERNAL
#define PLUGINNAME "InputSDL2"
#include "main.h"

#include <ctype.h>
#include <string>

Turbo::JSFunction IsKeyPressed(Turbo::JSArguments args){

    int sig[] = {Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig, true))
        return;

	SDL_PumpEvents();
	int key = args[0]->Int32Value();
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if(keystate[SDL_GetScancodeFromKey(key)])
        args.GetReturnValue().Set(v8::True(iso));
	else
        args.GetReturnValue().Set(v8::False(iso));
}

Turbo::JSFunction IsAnyKeyPressed(Turbo::JSArguments args){
	SDL_PumpEvents();
	int numkeys = 343;
	int *keys = &numkeys;
	const Uint8 *keystate = SDL_GetKeyboardState(keys);
	for(int i = 0; i<numkeys; i++){
		if(keystate[i])
            args.GetReturnValue().Set(v8::True(iso));
	}
    args.GetReturnValue().Set(v8::False(iso));
}
