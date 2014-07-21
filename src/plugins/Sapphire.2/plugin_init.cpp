#include "init.hpp"
#include "sapphire.h"

#include <SDL2/SDL.h>

v8::Isolate *iso;

int ref = 0;

const char *Init(uint32_t ID){

    if(ref!=0){
            int msg_box_err = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
"Sapphire Script Object List Error",
"An internal error has occured. The script function list was not initialized.",
NULL);

        assert(!msg_box_err);
        abort();
    }

    ref++;

    iso = v8::Isolate::GetCurrent();
    Sapphire::Init(ID);

    assert(Sapphire::GetFunctions());
    assert(Sapphire::GetVariables());

    return PLUGINNAME;
}

void Close(void){

    iso = NULL;
}

int GetNumFunctions(void){
    return Sapphire::GetFunctions()->GetNum();
}

int GetNumVariables(void){
    return Sapphire::GetVariables()->GetNum();
}

Turbo::JSFunctionArray GetFunctions(void){
    return const_cast<Turbo::JSFunctionArray>(Sapphire::GetFunctions()->GetA());
}

Turbo::JSValueArray GetVariables(void){
    return const_cast<Turbo::JSValueArray>(Sapphire::GetVariables()->GetA());
}

Turbo::JSNameArray GetFunctionNames(void){
    return Sapphire::GetFunctions()->GetNames();
}

Turbo::JSNameArray GetVariableNames(void){
    return Sapphire::GetVariables()->GetNames();
}
