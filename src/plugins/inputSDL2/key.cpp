#define INPUT_INTERNAL
#define PLUGINNAME "InputSDL2"
#include "main.h"

#include <ctype.h>
#include <string>

Turbo::JSFunction GetKey(Turbo::JSArguments args){

	SDL_Event keyevent1;
	SDL_PollEvent(&keyevent1);
	while(keyevent1.type!=SDL_KEYDOWN)
	{
        SDL_PeepEvents(&keyevent1, 1, SDL_ADDEVENT, 0, 0xFFFFFFFF);
        SDL_PollEvent(&keyevent1);
	}
	int key = keyevent1.key.keysym.sym;
	if(!key) {
        fprintf(stderr, "[" PLUGINNAME "] GetKey Error: You are probably using an unsupported keyboard layout\nSDLKey Error: %s\n", SDL_GetError());
    }
    //printf("The key pressed was %i, also known as %i.\n", key, SDL_SCANCODE_TO_KEYCODE(key));
    args.GetReturnValue().Set( v8::Integer::New(iso, key));
}

Turbo::JSFunction AreKeysLeft(Turbo::JSArguments args){

    SDL_PumpEvents();
    int remevents = SDL_PollEvent(NULL);

    args.GetReturnValue().Set(v8::Boolean::New(iso, (remevents>0)&&SDL_HasEvent(SDL_KEYDOWN)));

}

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
