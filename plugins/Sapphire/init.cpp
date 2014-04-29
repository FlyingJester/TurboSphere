#include "init.hpp"
#include "primitives.h"
#include "blendmodes.h"

#include "color.hpp"
#include "screen.hpp"

#include <SDL2/SDL.h>
#include <cassert>

inline const char *TS_BlendModeNames(size_t i){
    static const char *tnames[] = {
        "BLEND",
        "REPLACE",
        "RGB_ONLY",
        "ALPHA_ONLY",
        "ADD",
        "SUBTRACT",
        "MULTIPLY",
        "AVERAGE",
        "INVERT",
        "SQUARE"
    };
    assert(i<sizeof(tnames));

    return tnames[i];
}

template <class T>
inline void CheckSOL(Turbo::ScriptObjectList<T> *objectlist){
    if(!(objectlist->GetA()&&objectlist->GetNames())){

        int msg_box_err = SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
"Sapphire Script Object List Error",
"An internal error has occured. The script function list was not initialized.",
NULL);

        assert(!msg_box_err);
        abort();
    }
}

Turbo::ScriptObjectList<Turbo::JSCallback> *Functions = NULL;
Turbo::ScriptObjectList<Turbo::JSValue> *Variables = NULL;

const unsigned int num_funcs = 4;
const unsigned int num_vars = (
    NUM_BLENDMODES+
    0
);

void Sapphire::Init(uint32_t ID){

    InitColor(ID);
    InitPrimitives(ID);

    Functions = new Turbo::ScriptObjectList<Turbo::JSCallback>(num_funcs);
    Variables = new Turbo::ScriptObjectList<Turbo::JSValue>(num_vars);

    Functions->Set((Turbo::JSCallback)CreateColor, "Color", 0u);
    Functions->Set((Turbo::JSCallback)ColorToInt, "ColorToInt", 1u);
    Functions->Set((Turbo::JSCallback)Sapphire::Script::GetScreenWidth, "GetScreenWidth", 2u);
    Functions->Set((Turbo::JSCallback)Sapphire::Script::GetScreenHeight, "GetScreenHeight", 3u);

    //Set blendmodes.
    for(int i = 0; i<(int)num_vars; i++)
        Variables->Set(v8::Integer::New(iso, i), TS_BlendModeNames(i), i);

}

Turbo::ScriptObjectList<Turbo::JSCallback> *Sapphire::GetFunctions(void){
    CheckSOL(Functions);
    return Functions;
}

Turbo::ScriptObjectList<Turbo::JSValue>    *Sapphire::GetVariables(void){
    CheckSOL(Variables);
    return Variables;
}
