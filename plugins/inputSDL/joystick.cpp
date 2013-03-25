#include "joystick.h"

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

int checkJoystick(int jsNum, const char *ret){
    int numjoysticks = SDL_NumJoysticks();

    if(numjoysticks==0){
        ret = "No available joysticks.";
        return JOY_NO_JOYSTICKS;
    }

    if(jsNum>numjoysticks||jsNum<0){
        ret = "not a valid joystick number.";
        return JOY_OUT_OF_RANGE;
    }

    ret = "";
    return JOY_GOOD;
}

v8Function GetNumJoysticks(V8ARGS) {
    return v8::Number::New(SDL_NumJoysticks());
}

v8Function GetJoystickName(V8ARGS) {

    CHECK_ARG_INT(0, "GetNumJoySticks Error: Argument 0 is not a number.");

    int n = args[0]->Int32Value();
    const char *ret;
    int good = checkJoystick(n, ret);

    switch(good){
        case(JOY_NO_JOYSTICKS):
            THROWERROR_RANGE(((string)"GetJoystickName Error: "+string(ret)).c_str());
            break;
        case(JOY_OUT_OF_RANGE):
            THROWERROR_RANGE(((string)"GetJoystickName Error: Argument 0 is "+string(ret)).c_str());
            break;
        case(JOY_GOOD):
            return v8::String::New(SDL_JoystickName(n));
            break;
        default:
            THROWERROR("GetJoyStickName Error: Internal error.");
    }

}

v8Function GetNumJoystickButtons(V8ARGS) {
    CHECK_ARG_INT(0, "GetNumJoystickButtons Error: Argument 0 is not a number.");

    int n = args[0]->Int32Value();
    const char *ret;
    int good = checkJoystick(n, ret);

    switch(good){
        case(JOY_NO_JOYSTICKS):
            THROWERROR_RANGE(((string)"GetNumJoystickButtons Error: "+string(ret)).c_str());
            break;
        case(JOY_OUT_OF_RANGE):
            THROWERROR_RANGE(((string)"GetNumJoystickButtons Error: Argument 0 is "+string(ret)).c_str());
            break;
        case(JOY_GOOD):
            return v8::Integer::New(SDL_JoystickNumButtons(OpenJoysticks[n]));
            break;
        default:
            THROWERROR("GetNumJoystickButtons Error: Internal error.");
    }
}


v8Function GetNumJoystickAxes(V8ARGS) {
    CHECK_ARG_INT(0, "GetNumJoystickAxes Error: Argument 0 is not a number.");

    int n = args[0]->Int32Value();
    const char *ret;
    int good = checkJoystick(n, ret);

    switch(good){
        case(JOY_NO_JOYSTICKS):
            THROWERROR_RANGE(((string)"GetNumJoystickAxes Error: "+string(ret)).c_str());
            break;
        case(JOY_OUT_OF_RANGE):
            THROWERROR_RANGE(((string)"GetNumJoystickAxes Error: Argument 0 is "+string(ret)).c_str());
            break;
        case(JOY_GOOD):
            return v8::Integer::New(SDL_JoystickNumAxes(OpenJoysticks[n]));
            break;
        default:
            THROWERROR("GetNumJoystickAxes Error: Internal error.");
    }
}



v8Function IsJoystickButtonPressed(V8ARGS) {
    CHECK_ARG_INT(0, "IsJoystickButtonPressed Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "IsJoystickButtonPressed Error: Argument 1 is not a number.");

    int n = args[0]->Int32Value();
    int b = args[1]->Int32Value();
    const char *ret;
    int good = checkJoystick(n, ret);

    switch(good){
        case(JOY_NO_JOYSTICKS):
            THROWERROR_RANGE(((string)"IsJoystickButtonPressed Error: "+string(ret)).c_str());
            break;
        case(JOY_OUT_OF_RANGE):
            THROWERROR_RANGE(((string)"IsJoystickButtonPressed Error: Argument 0 is "+string(ret)).c_str());
            break;
        case(JOY_GOOD):
            if(b>SDL_JoystickNumButtons(OpenJoysticks[n])){
                THROWERROR_RANGE("IsJoystickButtonPressed Error: Argument 1 is not a valid button number.")
            }
            SDL_JoystickUpdate();
            return v8::Boolean::New(SDL_JoystickGetButton(OpenJoysticks[n], b));
            break;
        default:
            THROWERROR("IsJoystickButtonPressed Error: Internal error.");
    }
}

v8Function GetJoystickAxis(V8ARGS) {
    CHECK_ARG_INT(0, "GetJoystickAxis Error: Argument 0 is not a number.");
    CHECK_ARG_INT(1, "GetJoystickAxis Error: Argument 1 is not a number.");

    int n = args[0]->Int32Value();
    int a = args[1]->Int32Value();
    const char *ret;
    int good = checkJoystick(n, ret);

    switch(good){
        case(JOY_NO_JOYSTICKS):
            THROWERROR_RANGE(((string)"GetJoystickAxis Error: "+string(ret)).c_str());
            break;
        case(JOY_OUT_OF_RANGE):
            THROWERROR_RANGE(((string)"GetJoystickAxis Error: Argument 0 is "+string(ret)).c_str());
            break;
        case(JOY_GOOD):
            if(a>SDL_JoystickNumAxes(OpenJoysticks[n])){
                THROWERROR_RANGE("GetJoystickAxis Error: Argument 1 is not a valid button number.")
            }
            SDL_JoystickUpdate();
            return v8::Integer::New(SDL_JoystickGetAxis(OpenJoysticks[n], a));
            break;
        default:
            THROWERROR("GetJoystickAxis Error: Internal error.");
    }
}

