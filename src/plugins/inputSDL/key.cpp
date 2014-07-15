#define INPUT_INTERNAL
#include "main.h"

#include <ctype.h>
#include <string>

v8Function GetKey(V8ARGS)
{
	SDL_Event keyevent1;
	SDL_PollEvent(&keyevent1);
	while(keyevent1.type!=SDL_KEYDOWN)
	{
        SDL_PeepEvents(&keyevent1, 1, SDL_ADDEVENT, 0, 0xFFFFFFFF);
        SDL_PollEvent(&keyevent1);
	}
	int key = keyevent1.key.keysym.sym;
	if(!key) {
        printf("GetKey Recoverable Error: You are probably using an unsupported keyboard layout\nSDLKey Error: %s\n", SDL_GetError());
    }
    //printf("The key pressed was %i, also known as %i.\n", key, SDL_SCANCODE_TO_KEYCODE(key));
	return v8::Number::New(key);
}

v8Function AreKeysLeft(V8ARGS){

    SDL_PumpEvents();
    int remevents = SDL_PollEvent(NULL);

    return v8::Boolean::New((remevents>0)&&SDL_HasEvent(SDL_KEYDOWN));

}

v8Function IsKeyPressed(V8ARGS)
{
	if(args.Length()<1){
	    THROWERROR("IsKeyPressed Error: No arguments.")
    }
	CHECK_ARG_INT(0, "IsKeyPressed Error: arg 0 is not a number.");
	SDL_PumpEvents();
	int key = args[0]->Int32Value();
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if(keystate[SDL_GetScancodeFromKey(key)]){return v8::True();}
	else {return v8::False();}
}

v8Function IsAnyKeyPressed(V8ARGS)
{
	SDL_PumpEvents();
	int numkeys = 343;
	int *keys = &numkeys;
	const Uint8 *keystate = SDL_GetKeyboardState(keys);
	for(int i = 0; i<numkeys; i++){
		if(keystate[i]){
        return v8::True();
        }
	}
    return v8::False();
}