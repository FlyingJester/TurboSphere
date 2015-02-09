#pragma once
#include "input_main.hpp"
#include <t5.h>
#include <SDL2/SDL_events.h>

namespace InputSDL2{
template <int EventType, int(*tTranslate)(SDL_Event &aEvent)>
bool GetEvent(JSContext *ctx, unsigned argc, JS::Value *vp){

    SDL_Event event1;
    SDL_PollEvent(&event1);
    while(event1.type!=EventType)
    {
          SDL_PeepEvents(&event1, 1, SDL_ADDEVENT, 0, 0xFFFFFFFF);
          SDL_PollEvent(&event1);
    }

    int value = tTranslate(event1);
    if(!value) // Soft error. Report it, but it should not be fatal to script.
        t5::DataSource::StdOut()->WriteF(BRACKNAME, ' ', __func__, " Error ", SDL_GetError());
      
    CallArgsFromVp(argc, vp).rval().set(JS_NumberValue(value));
    return true;
}

template <int EventType>
bool AreEventsLeft(JSContext *ctx, unsigned argc, JS::Value *vp){

    SDL_PumpEvents();
    int remevents = SDL_PollEvent(NULL);

	CallArgsFromVp(argc, vp).rval().set(BOOLEAN_TO_JSVAL((remevents>0)&&SDL_HasEvent(EventType)));
    return true;
}

}
