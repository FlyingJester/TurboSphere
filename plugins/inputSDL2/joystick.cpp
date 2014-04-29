#define PLUGINNAME "InputSDL2"

#include "joystick.h"

extern v8::Isolate *iso;

#include <SDL2/SDL_joystick.h>

#include <string>

using std::string;

SDL_Joystick **OpenJoysticks;

void InitJoystick(){
    int numJoy =SDL_NumJoysticks();
    OpenJoysticks = (SDL_Joystick**)calloc(numJoy, sizeof(SDL_Joystick*));
    for(int i = 0; i<numJoy; i++){
        OpenJoysticks[i] = SDL_JoystickOpen(i);
    }
}

void CloseJoystick(){
    int numJoy =SDL_NumJoysticks();
    for(int i = 0; i<numJoy; i++){
        SDL_JoystickClose(OpenJoysticks[i]);
    }
    free(OpenJoysticks);
}

enum checkJoystickNumberStatus {JOY_GOOD, JOY_OUT_OF_RANGE, JOY_NO_JOYSTICKS};

int checkJoystick(int jsNum, const char **ret){
    int numjoysticks = SDL_NumJoysticks();

    if(numjoysticks==0){
        *ret = "No available joysticks.";
        return JOY_NO_JOYSTICKS;
    }

    if(jsNum>numjoysticks||jsNum<0){
        *ret = "not a valid joystick number.";
        return JOY_OUT_OF_RANGE;
    }

    return JOY_GOOD;
}

Turbo::JSFunction GetNumJoysticks(Turbo::JSArguments args) {
    args.GetReturnValue().Set(v8::Number::New(iso, SDL_NumJoysticks()));
}

Turbo::JSFunction GetJoystickName(Turbo::JSArguments args) {

    int sig[] = {Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig, true))
        return;

    int n = args[0]->Int32Value();
    const char *ret = NULL;
    int good = checkJoystick(n, &ret);

    switch(good){
        case(JOY_NO_JOYSTICKS):
            args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(iso, ((string)(("[" PLUGINNAME "] GetJoystickName Error: " ))+string(ret)).c_str())));
            break;
        case(JOY_OUT_OF_RANGE):
            args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(iso, ((string)("[" PLUGINNAME "] GetJoystickName  Error: Argument 0 is ")+string(ret)).c_str())));
            break;
        case(JOY_GOOD):
            args.GetReturnValue().Set(  v8::String::NewFromUtf8(iso, SDL_JoystickName(OpenJoysticks[n])));
            break;
        default:
            args.GetReturnValue().Set(v8::Exception::Error(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] GetJoystickName Internal Error.")));
    }

}

Turbo::JSFunction GetNumJoystickButtons(Turbo::JSArguments args) {

    int sig[] = {Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig, true))
        return;

    int n = args[0]->Int32Value();
    const char *ret = NULL;
    int good = checkJoystick(n, &ret);

    switch(good){
        case(JOY_NO_JOYSTICKS):
            args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(iso, (((string)("[" PLUGINNAME "] GetNumJoystickButtons Error: " )+string(ret)).c_str()))));
            break;
        case(JOY_OUT_OF_RANGE):
            args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(iso, ((string)("[" PLUGINNAME "] GetNumJoystickButtons  Error: Argument 0 is ")+string(ret)).c_str())));
            break;
        case(JOY_GOOD):
            args.GetReturnValue().Set(  v8::Integer::New(iso, SDL_JoystickNumButtons(OpenJoysticks[n])));
            break;
        default:
            args.GetReturnValue().Set(v8::Exception::Error(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] GetNumJoystickButtons Internal Error.")));
    }
}


Turbo::JSFunction GetNumJoystickAxes(Turbo::JSArguments args) {
    int sig[] = {Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig, true))
        return;

    int n = args[0]->Int32Value();
    const char *ret = NULL;
    int good = checkJoystick(n, &ret);

    switch(good){
        case(JOY_NO_JOYSTICKS):
            args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(iso, (((string)("[" PLUGINNAME "] GetNumJoystickAxes Error: " )+string(ret)).c_str()))));
            break;
        case(JOY_OUT_OF_RANGE):
            args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(iso, ((string)("[" PLUGINNAME "] GetNumJoystickAxes  Error: Argument 0 is ")+string(ret)).c_str())));
            break;
        case(JOY_GOOD):
            args.GetReturnValue().Set(  v8::Integer::New(iso, SDL_JoystickNumAxes(OpenJoysticks[n])));
            break;
        default:
            args.GetReturnValue().Set(v8::Exception::Error(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] GetNumJoystickAxes Internal Error.")));
    }
}



Turbo::JSFunction IsJoystickButtonPressed(Turbo::JSArguments args) {
    int sig[] = {Turbo::Int, Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 2, sig, true))
        return;

    int n = args[0]->Int32Value();
    int b = args[1]->Int32Value();
    const char *ret = NULL;
    int good = checkJoystick(n, &ret);

    switch(good){
        case(JOY_NO_JOYSTICKS):
            args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(iso, (((string)("[" PLUGINNAME "] IsJoystickButtonPressed Error: " )+string(ret)).c_str()))));
            break;
        case(JOY_OUT_OF_RANGE):
            args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(iso, ((string)("[" PLUGINNAME "] IsJoystickButtonPressed  Error: Argument 0 is ")+string(ret)).c_str())));
            break;
        case(JOY_GOOD):
            if(b>SDL_JoystickNumButtons(OpenJoysticks[n])){
                args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] IsJoystickButtonPressed Error: Argument 1 is not a valid button number.")));
                return;
            }
            SDL_JoystickUpdate();
            args.GetReturnValue().Set(  v8::Boolean::New(iso, SDL_JoystickGetButton(OpenJoysticks[n], b)));
            break;
        default:
            args.GetReturnValue().Set(v8::Exception::Error(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] IsJoystickButtonPressed Internal Error.")));
    }
}

Turbo::JSFunction GetJoystickAxis(Turbo::JSArguments args) {
    int sig[] = {Turbo::Int, Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 2, sig, true))
        return;

    int n = args[0]->Int32Value();
    int a = args[1]->Int32Value();
    const char *ret = NULL;
    int good = checkJoystick(n, &ret);

    switch(good){
        case(JOY_NO_JOYSTICKS):
            args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(iso, (((string)("[" PLUGINNAME "] GetJoystickAxis Error: " )+string(ret)).c_str()))));
            break;
        case(JOY_OUT_OF_RANGE):
            args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(iso, ((string)("[" PLUGINNAME "] GetJoystickAxis  Error: Argument 0 is ")+string(ret)).c_str())));
            break;
        case(JOY_GOOD):
            if(a>SDL_JoystickNumAxes(OpenJoysticks[n])){
                args.GetReturnValue().Set(v8::Exception::RangeError(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] GetJoystickAxis Error: Argument 1 is not a valid axis number.")));
                return;
            }
            SDL_JoystickUpdate();
            args.GetReturnValue().Set( v8::Integer::New(iso, SDL_JoystickGetAxis(OpenJoysticks[n], a)));
            break;
        default:
            args.GetReturnValue().Set(v8::Exception::Error(v8::String::NewFromUtf8(iso, "[" PLUGINNAME "] GetJoystickAxis Internal Error.")));
    }
}

