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

template<typename T, T (*callback)(SDL_Joystick*)>
void TS_JS_ReturnWrapper(Turbo::JSArguments args, SDL_Joystick *js){
    args.GetReturnValue().Set(callback(js));
}

template<>
void TS_JS_ReturnWrapper<const char *, SDL_JoystickName>(Turbo::JSArguments args, SDL_Joystick *js){
    args.GetReturnValue().Set(v8::String::NewFromUtf8(args.GetIsolate(), SDL_JoystickName(js)));
}

template<typename T, T (*callback)(SDL_Joystick*)>
void TS_HandleJoystickError(Turbo::JSArguments args, SDL_Joystick *js, int err, const char *ret, const std::string func){
    switch(err){
        case(JOY_NO_JOYSTICKS):
            Turbo::SetError(args, (string)(("[" PLUGINNAME "] "+ func + " Error: " ))+string(ret), v8::Exception::RangeError);
            break;
        case(JOY_OUT_OF_RANGE):
            Turbo::SetError(args, (string)("[" PLUGINNAME "] "+ func + "  Error: Argument 0 is ")+string(ret), v8::Exception::RangeError);
            break;
        case(JOY_GOOD):
            TS_JS_ReturnWrapper<T, callback>(args, js);
            break;
        default:
            Turbo::SetError(args, "[" PLUGINNAME "] "+ func + " Internal Error.");
    }
}

Turbo::JSFunction GetJoystickName(Turbo::JSArguments args) {

    int sig[] = {Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig, true))
        return;

    int n = args[0]->Int32Value();
    const char *ret = NULL;
    int good = checkJoystick(n, &ret);

    TS_HandleJoystickError<const char *, SDL_JoystickName>(args, OpenJoysticks[n], good, ret, __func__);


}

Turbo::JSFunction GetNumJoystickButtons(Turbo::JSArguments args) {

    int sig[] = {Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig, true))
        return;

    int n = args[0]->Int32Value();
    const char *ret = NULL;
    int good = checkJoystick(n, &ret);

    TS_HandleJoystickError<int, SDL_JoystickNumButtons>(args, OpenJoysticks[n], good, ret, __func__);

}


Turbo::JSFunction GetNumJoystickAxes(Turbo::JSArguments args) {
    int sig[] = {Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig, true))
        return;

    int n = args[0]->Int32Value();
    const char *ret = NULL;
    int good = checkJoystick(n, &ret);

    TS_HandleJoystickError<int, SDL_JoystickNumAxes>(args, OpenJoysticks[n], good, ret, __func__);

}



Turbo::JSFunction IsJoystickButtonPressed(Turbo::JSArguments args) {
    int sig[] = {Turbo::Int, Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 2, sig, true))
        return;

    int n = args[0]->Int32Value();
    int b = args[1]->Int32Value();
    const char *ret = NULL;
    int good = checkJoystick(n, &ret);


    if(good==JOY_GOOD){
        if(b>SDL_JoystickNumButtons(OpenJoysticks[n])){
            Turbo::SetError(args, "[" PLUGINNAME "] IsJoystickButtonPressed Error: Argument 1 is not a valid button number.", v8::Exception::RangeError);
            return;
        }
        SDL_JoystickUpdate();
        args.GetReturnValue().Set( v8::Boolean::New(iso, SDL_JoystickGetButton(OpenJoysticks[n], b)));
        return;
    }

    TS_HandleJoystickError<int, nullptr>(args, nullptr, good, ret, __func__);

}

Turbo::JSFunction GetJoystickAxis(Turbo::JSArguments args) {
    int sig[] = {Turbo::Int, Turbo::Int, 0};

    if(!Turbo::CheckArg::CheckSig(args, 2, sig, true))
        return;

    int n = args[0]->Int32Value();
    int a = args[1]->Int32Value();
    const char *ret = NULL;
    int good = checkJoystick(n, &ret);



    if(good==JOY_GOOD){
        if(a>SDL_JoystickNumAxes(OpenJoysticks[n])){
            Turbo::SetError(args, "[" PLUGINNAME "] GetJoystickAxis Error: Argument 1 is not a valid button number.", v8::Exception::RangeError);
            return;
        }
        SDL_JoystickUpdate();
        args.GetReturnValue().Set(v8::Integer::New(args.GetIsolate(), SDL_JoystickGetAxis(OpenJoysticks[n], a)));
        return;
    }

    TS_HandleJoystickError<int, nullptr>(args, nullptr, good, ret, __func__);

}

