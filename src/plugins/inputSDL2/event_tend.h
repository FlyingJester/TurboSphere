#ifndef INPUTSDL2_EVENT_TEND_HEAD
#define INPUTSDL2_EVENT_TEND_HEAD

#include "main.h"

template <int EventType, int(*tTranslate)(SDL_Event &aEvent)>
Turbo::JSFunction GetEvent(Turbo::JSArguments args){

    SDL_Event event1;
    SDL_PollEvent(&event1);
    while(event1.type!=EventType)
    {
          SDL_PeepEvents(&event1, 1, SDL_ADDEVENT, 0, 0xFFFFFFFF);
          SDL_PollEvent(&event1);
    }

    int value = tTranslate(event1);
    if(!value)
      fprintf(stderr, "[" PLUGINNAME "] %s Error: %s\n", __func__, SDL_GetError());
    args.GetReturnValue().Set( v8::Integer::New(v8::Isolate::GetCurrent(), value));
}

template <int EventType>
Turbo::JSFunction AreEventsLeft(Turbo::JSArguments args){

    SDL_PumpEvents();
    int remevents = SDL_PollEvent(NULL);

    args.GetReturnValue().Set(v8::Boolean::New(v8::Isolate::GetCurrent(), (remevents>0)&&SDL_HasEvent(EventType)));

}

#endif
