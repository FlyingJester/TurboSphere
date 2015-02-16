#include "input_main.hpp"
#include "mouse.hpp"
#include "key.hpp"
#include "joystick.hpp"
#include "event_tend.hpp"
#include <array>
#include <SDL2/SDL.h>

namespace InputSDL2{

int GetKeyEvent(SDL_Event &aEvent){return aEvent.key.keysym.sym;}
int GetClickEvent(SDL_Event &aEvent){return aEvent.button.button+1;}
int GetWheelEvent(SDL_Event &aEvent){
    return ((aEvent.wheel.y<<1)-1) + ((aEvent.wheel.x<<1)+1);
}

const unsigned num_funcs = 17;

std::array<JSNative, num_funcs> script_function_list = {{
    Script::IsKeyPressed,
    Script::IsAnyKeyPressed,
    GetEvent<(int)SDL_KEYDOWN, GetKeyEvent>,
    GetEvent<(int)SDL_MOUSEBUTTONDOWN, GetClickEvent>,
    GetEvent<(int)SDL_MOUSEWHEEL, GetWheelEvent>,
    AreEventsLeft<(int)SDL_KEYDOWN>,
    AreEventsLeft<(int)SDL_MOUSEBUTTONDOWN>,
    AreEventsLeft<(int)SDL_MOUSEWHEEL>,
    Script::GetMouseX,
    Script::GetMouseY,
    Script::IsMouseButtonPressed,
    Script::GetNumJoysticks,
    Script::GetJoystickName,
    Script::GetNumJoystickButtons,
    Script::GetNumJoystickAxes,
    Script::IsJoystickButtonPressed,
    Script::GetJoystickAxis
}};

std::array<const char * const, num_funcs>  script_function_name_list = {{
    "IsKeyPressed",
    "IsAnyKeyPressed",
    "GetKey",
    "GetClick",
    "GetMouseWheelEvent",
    "AreKeysLeft",
    "AreClicksLeft",
    "GetNumMouseWheelEvents",
    "GetMouseX",
    "GetMouseY",
    "IsMouseButtonPressed",
    "GetNumJoysticks",
    "GetJoystickName",
    "GetNumJoystickButtons",
    "GetNumJoystickAxes",
    "IsJoystickButtonPressed",
    "GetJoystickAxis"
}};

}

const char *Init(JSContext *ctx, unsigned ID){

	if(SDL_WasInit(0)==0){
        SDL_Init(SDL_INIT_JOYSTICK);
    }
    else if(SDL_WasInit(SDL_INIT_JOYSTICK)==0){
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    }

    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);

    InputSDL2::InitJoystick(ctx);

    return PLUGINNAME;
}

void Close(JSContext *ctx){
    InputSDL2::CloseJoystick(ctx);
}

int NumFunctions(JSContext *ctx){
    assert(ctx);
    return InputSDL2::script_function_list.size();
}

JSNative GetFunction(JSContext *ctx, int n){
    assert(ctx);
    assert(n<InputSDL2::script_function_list.size());
    return InputSDL2::script_function_list[n];
}

const char *GetFunctionName(JSContext *ctx, int n){
    assert(ctx);
    assert(n<InputSDL2::script_function_name_list.size());
    return InputSDL2::script_function_name_list[n];
}

int NumVariables(JSContext *ctx){
    return NUM_KEYS+7;
}
void GetVariable(JSContext *ctx, int n, JS::MutableHandleValue val){
    if(n>=NUM_KEYS){
        if(n-NUM_KEYS<3)
            val.set(JS_NumberValue(SDL_BUTTON((n-NUM_KEYS)+1)));
        else if(n-NUM_KEYS<7)
            val.set(JS_NumberValue(n-NUM_KEYS-3));
        else // N out of range.
           assert(false);     
    }
    else{
        val.set(JS_NumberValue(InputSDL2::key_numbers[n]));
    }
}
const char *GetVariableName(JSContext *ctx, int n){
    if(n>=NUM_KEYS){
        switch(n-NUM_KEYS){
            case 0:
            return "MOUSE_LEFT";
            case 1:
            return "MOUSE_RIGHT";
            case 2:
            return "MOUSE_MIDDLE";
            case 3:
            return "MOUSE_WHEEL_UP";
            case 4:
            return "MOUSE_WHEEL_DOWN";
            case 5:
            return "MOUSE_WHEEL_LEFT";
            case 6:
            return "MOUSE_WHEEL_RIGHT";
            default: // N out of range.
            assert(false);
        }            
    }
    else{
        return InputSDL2::key_names[n];
    }
}
