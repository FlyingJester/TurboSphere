#define INPUT_INTERNAL
#include "main.h"
#include "mouse.h"
#include "key.h"
#include "joystick.h"
#include <stdio.h>

SDL_Event keyevent;

v8Function IsKeyPressed(V8ARGS);
v8Function IsAnyKeyPressed(V8ARGS);
v8Function GetKey(V8ARGS);
v8Function AreKeysLeft(V8ARGS);
//v8::Handle<v8::Value> GetKeyString(const v8::Arguments& args);

int numerate(bool reset){
    static uint64_t i = 0;
    if(reset) {
        i = 0;
        return 0;
    }
    i++;
    return (int)(i-1);
}

void * IsKeyPressedPointer            = V8FUNCPOINTER(IsKeyPressed);
void * IsAnyKeyPressedPointer         = V8FUNCPOINTER(IsAnyKeyPressed);
void * GetKeyPointer                  = V8FUNCPOINTER(GetKey);
void * AreKeysLeftPointer             = V8FUNCPOINTER(AreKeysLeft);
void * GetMouseXPointer               = V8FUNCPOINTER(GetMouseX);
void * GetMouseYPointer               = V8FUNCPOINTER(GetMouseY);
void * IsMouseButtonPressedPointer    = V8FUNCPOINTER(IsMouseButtonPressed);
void * GetNumJoysticksPointer         = V8FUNCPOINTER(GetNumJoysticks);
void * GetJoystickNamePointer         = V8FUNCPOINTER(GetJoystickName);
void * GetNumJoystickButtonsPointer   = V8FUNCPOINTER(GetNumJoystickButtons);
void * GetNumJoystickAxesPointer      = V8FUNCPOINTER(GetNumJoystickAxes);
void * IsJoystickButtonPressedPointer = V8FUNCPOINTER(IsJoystickButtonPressed);
void * GetJoystickAxisPointer         = V8FUNCPOINTER(GetJoystickAxis);

initFunction Init(void){
	if(SDL_WasInit(0)==0){
        SDL_Init(SDL_INIT_JOYSTICK);
        //atexit(SDL_Quit);
    }
    else if(SDL_WasInit(SDL_INIT_JOYSTICK)==0){
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    }
    //SDL_EnableUNICODE(SDL_ENABLE);
//    SDL_EventState(SDL_KEYUP, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
    SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
    //SDL_EventState(SDL_VIDEOEXPOSE, SDL_IGNORE);
    InitJoystick();

    printf("[inputSDL] Info: Event state setup succeeded.\n");

    return (initFunction)"inputSDL";
}

int GetNumFunctions(){
    return NUMFUNCS;
}

functionArray GetFunctions(){
    numerate(true);
    functionArray funcs = (functionArray)calloc(13, 8);
    funcs[numerate(false)]=IsKeyPressedPointer;             //1
    funcs[numerate(false)]=IsAnyKeyPressedPointer;          //2
    funcs[numerate(false)]=GetKeyPointer;                   //3
    funcs[numerate(false)]=AreKeysLeftPointer;              //4
    funcs[numerate(false)]=GetMouseXPointer;                //5
    funcs[numerate(false)]=GetMouseYPointer;                //6
    funcs[numerate(false)]=IsMouseButtonPressedPointer;     //7
    funcs[numerate(false)]=GetNumJoysticksPointer;          //8
    funcs[numerate(false)]=GetJoystickNamePointer;          //9
    funcs[numerate(false)]=GetNumJoystickButtonsPointer;    //10
    funcs[numerate(false)]=GetNumJoystickAxesPointer;       //11
    funcs[numerate(false)]=IsJoystickButtonPressedPointer;  //12
    funcs[numerate(false)]=GetJoystickAxisPointer;          //13
    return funcs;
}

nameArray GetFunctionNames(){
    numerate(true);
//    nameArray names = (nameArray)calloc(GetNumFunctions(), sizeof(functionName));
    nameArray names = (nameArray)calloc(13, 8);
    names[numerate(false)]=(functionName)"IsKeyPressed";
    names[numerate(false)]=(functionName)"IsAnyKeyPressed";
    names[numerate(false)]=(functionName)"GetKey";
    names[numerate(false)]=(functionName)"AreKeysLeft";
    names[numerate(false)]=(functionName)"GetMouseX";
    names[numerate(false)]=(functionName)"GetMouseY";
    names[numerate(false)]=(functionName)"IsMouseButtonPressed";
    names[numerate(false)]=(functionName)"GetNumJoysticks";
    names[numerate(false)]=(functionName)"GetJoystickName";
    names[numerate(false)]=(functionName)"GetNumJoystickButtons";
    names[numerate(false)]=(functionName)"GetNumJoystickAxes";
    names[numerate(false)]=(functionName)"IsJoystickButtonPressed";
    names[numerate(false)]=(functionName)"GetJoystickAxis";
    return names;
}

int GetNumVariables(void){
    return NUMVARS;
}

void Close(){
    CloseJoystick();
}

v8FunctionArray GetVariables(void){

    numerate(true);
    v8FunctionArray vars = (v8FunctionArray)calloc(NUMVARS, sizeof(v8Function));

    vars[numerate(false)]=v8::Number::New(SDLK_RETURN);

	vars[numerate(false)]=v8::Number::New(SDLK_a);

	vars[numerate(false)]=v8::Number::New(SDLK_b);

	vars[numerate(false)]=v8::Number::New(SDLK_c);

	vars[numerate(false)]=v8::Number::New(SDLK_d);

	vars[numerate(false)]=v8::Number::New(SDLK_e);

    vars[numerate(false)]=v8::Number::New(SDLK_f);

	vars[numerate(false)]=v8::Number::New(SDLK_g);

	vars[numerate(false)]=v8::Number::New(SDLK_h);

	vars[numerate(false)]=v8::Number::New(SDLK_i);

	vars[numerate(false)]=v8::Number::New(SDLK_j);

	vars[numerate(false)]=v8::Number::New(SDLK_k);

	vars[numerate(false)]=v8::Number::New(SDLK_l);

	vars[numerate(false)]=v8::Number::New(SDLK_m);

	vars[numerate(false)]=v8::Number::New(SDLK_n);

	vars[numerate(false)]=v8::Number::New(SDLK_o);

	vars[numerate(false)]=v8::Number::New(SDLK_p);

	vars[numerate(false)]=v8::Number::New(SDLK_q);

	vars[numerate(false)]=v8::Number::New(SDLK_r);

	vars[numerate(false)]=v8::Number::New(SDLK_s);

	vars[numerate(false)]=v8::Number::New(SDLK_t);

	vars[numerate(false)]=v8::Number::New(SDLK_u);

	vars[numerate(false)]=v8::Number::New(SDLK_v);

	vars[numerate(false)]=v8::Number::New(SDLK_w);

	vars[numerate(false)]=v8::Number::New(SDLK_x);

	vars[numerate(false)]=v8::Number::New(SDLK_y);

	vars[numerate(false)]=v8::Number::New(SDLK_z);

	vars[numerate(false)]=v8::Number::New(SDLK_TAB);

	vars[numerate(false)]=v8::Number::New(SDLK_ESCAPE);

	vars[numerate(false)]=v8::Number::New(SDLK_F1);

	vars[numerate(false)]=v8::Number::New(SDLK_F2);

	vars[numerate(false)]=v8::Number::New(SDLK_F3);

	vars[numerate(false)]=v8::Number::New(SDLK_F4);

	vars[numerate(false)]=v8::Number::New(SDLK_F5);

	vars[numerate(false)]=v8::Number::New(SDLK_F6);

	vars[numerate(false)]=v8::Number::New(SDLK_F7);

	vars[numerate(false)]=v8::Number::New(SDLK_F8);

	vars[numerate(false)]=v8::Number::New(SDLK_F9);

	vars[numerate(false)]=v8::Number::New(SDLK_F10);

	vars[numerate(false)]=v8::Number::New(SDLK_F11);

	vars[numerate(false)]=v8::Number::New(SDLK_F12);

	vars[numerate(false)]=v8::Number::New(SDLK_F13);

	vars[numerate(false)]=v8::Number::New(SDLK_F14);

	vars[numerate(false)]=v8::Number::New(SDLK_F15);

	vars[numerate(false)]=v8::Number::New(SDLK_BACKQUOTE);

	vars[numerate(false)]=v8::Number::New(SDLK_0);

	vars[numerate(false)]=v8::Number::New(SDLK_1);

	vars[numerate(false)]=v8::Number::New(SDLK_2);

	vars[numerate(false)]=v8::Number::New(SDLK_3);

	vars[numerate(false)]=v8::Number::New(SDLK_4);

	vars[numerate(false)]=v8::Number::New(SDLK_5);

	vars[numerate(false)]=v8::Number::New(SDLK_6);

	vars[numerate(false)]=v8::Number::New(SDLK_7);

	vars[numerate(false)]=v8::Number::New(SDLK_8);

	vars[numerate(false)]=v8::Number::New(SDLK_9);

	vars[numerate(false)]=v8::Number::New(SDLK_MINUS);

	vars[numerate(false)]=v8::Number::New(SDLK_EQUALS);

	vars[numerate(false)]=v8::Number::New(SDLK_BACKSPACE);

	vars[numerate(false)]=v8::Number::New(SDLK_LSHIFT);

	vars[numerate(false)]=v8::Number::New(SDLK_RSHIFT);

	vars[numerate(false)]=v8::Number::New(SDLK_CAPSLOCK);

	vars[numerate(false)]=v8::Number::New(SDLK_NUMLOCK);

	vars[numerate(false)]=v8::Number::New(SDLK_SCROLLOCK);

	vars[numerate(false)]=v8::Number::New(SDLK_LCTRL);

	vars[numerate(false)]=v8::Number::New(SDLK_RCTRL);

	vars[numerate(false)]=v8::Number::New(SDLK_LALT);

	vars[numerate(false)]=v8::Number::New(SDLK_RALT);

	vars[numerate(false)]=v8::Number::New(SDLK_SPACE);

	vars[numerate(false)]=v8::Number::New(SDLK_LEFTBRACKET);

	vars[numerate(false)]=v8::Number::New(SDLK_RIGHTBRACKET);

	vars[numerate(false)]=v8::Number::New(SDLK_SEMICOLON);

	vars[numerate(false)]=v8::Number::New(SDLK_QUOTE);

	vars[numerate(false)]=v8::Number::New(SDLK_COMMA);

	vars[numerate(false)]=v8::Number::New(SDLK_PERIOD);

	vars[numerate(false)]=v8::Number::New(SDLK_SLASH);

	vars[numerate(false)]=v8::Number::New(SDLK_BACKSLASH);

	vars[numerate(false)]=v8::Number::New(SDLK_INSERT);

	vars[numerate(false)]=v8::Number::New(SDLK_DELETE);

	vars[numerate(false)]=v8::Number::New(SDLK_HOME);

	vars[numerate(false)]=v8::Number::New(SDLK_END);

	vars[numerate(false)]=v8::Number::New(SDLK_PAGEUP);

	vars[numerate(false)]=v8::Number::New(SDLK_PAGEDOWN);

	vars[numerate(false)]=v8::Number::New(SDLK_UP);

	vars[numerate(false)]=v8::Number::New(SDLK_RIGHT);

	vars[numerate(false)]=v8::Number::New(SDLK_DOWN);

	vars[numerate(false)]=v8::Number::New(SDLK_LEFT);

	vars[numerate(false)]=v8::Number::New(SDLK_KP0);

	vars[numerate(false)]=v8::Number::New(SDLK_KP1);

	vars[numerate(false)]=v8::Number::New(SDLK_KP2);

	vars[numerate(false)]=v8::Number::New(SDLK_KP3);

	vars[numerate(false)]=v8::Number::New(SDLK_KP4);

	vars[numerate(false)]=v8::Number::New(SDLK_KP5);

	vars[numerate(false)]=v8::Number::New(SDLK_KP6);

	vars[numerate(false)]=v8::Number::New(SDLK_KP7);

	vars[numerate(false)]=v8::Number::New(SDLK_KP8);

	vars[numerate(false)]=v8::Number::New(SDLK_KP9);

	vars[numerate(false)]=v8::Number::New(SDLK_KP_PERIOD);

	vars[numerate(false)]=v8::Number::New(SDLK_KP_DIVIDE);

	vars[numerate(false)]=v8::Number::New(SDLK_KP_MULTIPLY);

	vars[numerate(false)]=v8::Number::New(SDLK_KP_MINUS);

	vars[numerate(false)]=v8::Number::New(SDLK_KP_EQUALS);

	vars[numerate(false)]=v8::Number::New(SDLK_RMETA);

	vars[numerate(false)]=v8::Number::New(SDLK_LMETA);

	vars[numerate(false)]=v8::Number::New(SDLK_RSUPER);

	vars[numerate(false)]=v8::Number::New(SDLK_LSUPER);

	vars[numerate(false)]=v8::Number::New(SDLK_BREAK);

	vars[numerate(false)]=v8::Number::New(SDLK_MENU);

	vars[numerate(false)]=v8::Number::New(SDLK_POWER);

	vars[numerate(false)]=v8::Number::New(SDLK_EURO);

//Mouse Button Constants
	vars[numerate(false)]=v8::Number::New(0);

	vars[numerate(false)]=v8::Number::New(1);

	vars[numerate(false)]=v8::Number::New(2);

	vars[numerate(false)]=v8::Number::New(3);

	vars[numerate(false)]=v8::Number::New(4);

	vars[numerate(false)]=v8::Number::New(5);

	return vars;

}


nameArray GetVariableNames(void){

    numerate(true);
    nameArray varnames = (nameArray)calloc(NUMVARS, sizeof(variableName));

    if((varnames==NULL)&&(NUMVARS>0)){
        printf("Out of memory.\n");
        exit(1);
    }

    //Keyboard

	varnames[numerate(false)]=(char*)"KEY_ENTER";

	varnames[numerate(false)]=(char*)"KEY_A";

	varnames[numerate(false)]=(char*)"KEY_B";

	varnames[numerate(false)]=(char*)"KEY_C";

	varnames[numerate(false)]=(char*)"KEY_D";

	varnames[numerate(false)]=(char*)"KEY_E";

	varnames[numerate(false)]=(char*)"KEY_F";

	varnames[numerate(false)]=(char*)"KEY_G";

	varnames[numerate(false)]=(char*)"KEY_H";

	varnames[numerate(false)]=(char*)"KEY_I";

	varnames[numerate(false)]=(char*)"KEY_J";

	varnames[numerate(false)]=(char*)"KEY_K";

	varnames[numerate(false)]=(char*)"KEY_L";

	varnames[numerate(false)]=(char*)"KEY_M";

	varnames[numerate(false)]=(char*)"KEY_N";

	varnames[numerate(false)]=(char*)"KEY_O";

	varnames[numerate(false)]=(char*)"KEY_P";

	varnames[numerate(false)]=(char*)"KEY_Q";

	varnames[numerate(false)]=(char*)"KEY_R";

	varnames[numerate(false)]=(char*)"KEY_S";

	varnames[numerate(false)]=(char*)"KEY_T";

	varnames[numerate(false)]=(char*)"KEY_U";

	varnames[numerate(false)]=(char*)"KEY_V";

	varnames[numerate(false)]=(char*)"KEY_W";

	varnames[numerate(false)]=(char*)"KEY_X";

	varnames[numerate(false)]=(char*)"KEY_Y";

	varnames[numerate(false)]=(char*)"KEY_Z";

	varnames[numerate(false)]=(char*)"KEY_TAB";

	varnames[numerate(false)]=(char*)"KEY_ESCAPE";

	varnames[numerate(false)]=(char*)"KEY_F1";

	varnames[numerate(false)]=(char*)"KEY_F2";

	varnames[numerate(false)]=(char*)"KEY_F3";

	varnames[numerate(false)]=(char*)"KEY_F4";

	varnames[numerate(false)]=(char*)"KEY_F5";

	varnames[numerate(false)]=(char*)"KEY_F6";

	varnames[numerate(false)]=(char*)"KEY_F7";

	varnames[numerate(false)]=(char*)"KEY_F8";

	varnames[numerate(false)]=(char*)"KEY_F9";

	varnames[numerate(false)]=(char*)"KEY_F10";

	varnames[numerate(false)]=(char*)"KEY_F11";

	varnames[numerate(false)]=(char*)"KEY_F12";

	varnames[numerate(false)]=(char*)"KEY_F13";

	varnames[numerate(false)]=(char*)"KEY_F14";

	varnames[numerate(false)]=(char*)"KEY_F15";

	varnames[numerate(false)]=(char*)"KEY_TILDE";

	varnames[numerate(false)]=(char*)"KEY_0";

	varnames[numerate(false)]=(char*)"KEY_1";

	varnames[numerate(false)]=(char*)"KEY_2";

	varnames[numerate(false)]=(char*)"KEY_3";

	varnames[numerate(false)]=(char*)"KEY_4";

	varnames[numerate(false)]=(char*)"KEY_5";

	varnames[numerate(false)]=(char*)"KEY_6";

	varnames[numerate(false)]=(char*)"KEY_7";

	varnames[numerate(false)]=(char*)"KEY_8";

	varnames[numerate(false)]=(char*)"KEY_9";

	varnames[numerate(false)]=(char*)"KEY_MINUS";

	varnames[numerate(false)]=(char*)"KEY_EQUALS";

	varnames[numerate(false)]=(char*)"KEY_BACKSPACE";

	varnames[numerate(false)]=(char*)"KEY_SHIFT";

	varnames[numerate(false)]=(char*)"KEY_RSHIFT";

	varnames[numerate(false)]=(char*)"KEY_CAPSLOCK";

	varnames[numerate(false)]=(char*)"KEY_NUMLOCK";

	varnames[numerate(false)]=(char*)"KEY_SCROLLOCK";

	varnames[numerate(false)]=(char*)"KEY_CTRL";

	varnames[numerate(false)]=(char*)"KEY_RCTRL";

	varnames[numerate(false)]=(char*)"KEY_ALT";

	varnames[numerate(false)]=(char*)"KEY_RALT";

	varnames[numerate(false)]=(char*)"KEY_SPACE";

	varnames[numerate(false)]=(char*)"KEY_OPENBRACE";

	varnames[numerate(false)]=(char*)"KEY_CLOSEBRACE";

	varnames[numerate(false)]=(char*)"KEY_SEMICOLON";

	varnames[numerate(false)]=(char*)"KEY_APOSTROPHE";

	varnames[numerate(false)]=(char*)"KEY_COMMA";

	varnames[numerate(false)]=(char*)"KEY_PERIOD";

	varnames[numerate(false)]=(char*)"KEY_SLASH";

	varnames[numerate(false)]=(char*)"KEY_BACKSLASH";

	varnames[numerate(false)]=(char*)"KEY_INSERT";

	varnames[numerate(false)]=(char*)"KEY_DELETE";

	varnames[numerate(false)]=(char*)"KEY_HOME";

	varnames[numerate(false)]=(char*)"KEY_END";

	varnames[numerate(false)]=(char*)"KEY_PAGEUP";

	varnames[numerate(false)]=(char*)"KEY_PAGEDOWN";

	varnames[numerate(false)]=(char*)"KEY_UP";

	varnames[numerate(false)]=(char*)"KEY_RIGHT";

	varnames[numerate(false)]=(char*)"KEY_DOWN";

	varnames[numerate(false)]=(char*)"KEY_LEFT";

	varnames[numerate(false)]=(char*)"KEY_NUM_0";

	varnames[numerate(false)]=(char*)"KEY_NUM_1";

	varnames[numerate(false)]=(char*)"KEY_NUM_2";

	varnames[numerate(false)]=(char*)"KEY_NUM_3";

	varnames[numerate(false)]=(char*)"KEY_NUM_4";

	varnames[numerate(false)]=(char*)"KEY_NUM_5";

	varnames[numerate(false)]=(char*)"KEY_NUM_6";

	varnames[numerate(false)]=(char*)"KEY_NUM_7";

	varnames[numerate(false)]=(char*)"KEY_NUM_8";

	varnames[numerate(false)]=(char*)"KEY_NUM_9";

	varnames[numerate(false)]=(char*)"KEY_NUM_PERIOD";

	varnames[numerate(false)]=(char*)"KEY_NUM_DIVIDE";

	varnames[numerate(false)]=(char*)"KEY_NUM_MULTIPLY";

	varnames[numerate(false)]=(char*)"KEY_NUM_MINUS";

	varnames[numerate(false)]=(char*)"KEY_NUM_EQUALS";

	varnames[numerate(false)]=(char*)"KEY_RMETA";

	varnames[numerate(false)]=(char*)"KEY_LMETA";

	varnames[numerate(false)]=(char*)"KEY_RSUPER";

	varnames[numerate(false)]=(char*)"KEY_LSUPER";

	varnames[numerate(false)]=(char*)"KEY_BREAK";

	varnames[numerate(false)]=(char*)"KEY_MENU";

	varnames[numerate(false)]=(char*)"KEY_POWER";

	varnames[numerate(false)]=(char*)"KEY_EURO";

    //Mouse

    varnames[numerate(false)]=(char*)"MOUSE_LEFT"; //0

	varnames[numerate(false)]=(char*)"MOUSE_MIDDLE"; //1

	varnames[numerate(false)]=(char*)"MOUSE_RIGHT"; //2

	varnames[numerate(false)]=(char*)"MOUSE_SUP"; //3

	varnames[numerate(false)]=(char*)"MOUSE_SDOWN"; //4

	varnames[numerate(false)]=(char*)"MOUSE_SDIS"; //5

    return varnames;

}
