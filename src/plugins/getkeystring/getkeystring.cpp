#define PLUGINNAME "GetKeyString"
#include <pluginsdk/plugin.h>
#include "getkeystring.h"
#include <string>

Turbo::JSFunction GetKeyString(Turbo::JSArguments args);

Turbo::JSCallback GetKeyStringPointer = (Turbo::JSCallback)((Turbo::JSFunction (*)(Turbo::JSArguments))(GetKeyString));

static Turbo::JSFunctionArray funcs = NULL;
static Turbo::JSNameArray names = NULL;


const char * Init(void){
    funcs = (Turbo::JSFunctionArray)calloc(1, sizeof(Turbo::JSCallback));
    funcs[0]=GetKeyStringPointer;

    names = (Turbo::JSNameArray)calloc(1, sizeof(Turbo::JSFunctionName));
    names[0]=(Turbo::JSFunctionName)"GetKeyString";

    return (const char*)"GetKeyString";
}


int GetNumFunctions(void){
    return 1;
}

void Close(){
    free(names);
    free(funcs);
}

Turbo::JSFunctionArray GetFunctions(void){
    return funcs;
}

Turbo::JSNameArray GetFunctionNames(void){
    return names;
}

int GetNumVariables(void){
    return 0;
}

Turbo::JSValueArray GetVariables(void){
    return NULL;
}

Turbo::JSNameArray  GetVariableNames(void){
    return NULL;
}

Turbo::JSFunction GetKeyString(Turbo::JSArguments args){

    int sig[] = {Turbo::Int, Turbo::Bool, 0};

    if(!Turbo::CheckArg::CheckSig(args, 1, sig))
        return;

    bool shift = false;

    if(!Turbo::CheckArg::CheckSig(args, 2, sig, false)){
        if(args[1]->v8::Value::BooleanValue()){
            shift = true;
        }
	}

    unsigned int keynum = args[0]->v8::Value::Int32Value();
    if (keynum>255){
        args.GetReturnValue().Set( v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ""));
        return;
    }

    if(shift){
        keynum = toupper(keynum);
    }

    std::string key(1, keynum);
    args.GetReturnValue().Set( v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), key.c_str()));
}
