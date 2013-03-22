#include "../common/plugin.h"
#include "getkeystring.h"
#include <string>

v8::Handle<v8::Value> GetKeyString(const v8::Arguments& args);

void * GetKeyStringPointer = (void *)((v8::Handle<v8::Value> (*)(const v8::Arguments& args))(GetKeyString));

const char * Init(void){
    return (char*)"GetKeyString";
}


int GetNumFunctions(void){
    return 1;
}

void Close(){

}

void ** GetFunctions(void){
    void ** funcs = (void**)calloc(1, sizeof(void*));
    funcs[0]=GetKeyStringPointer;
    return funcs;
}

const char ** GetFunctionNames(void){
    const char ** names = (const char**)calloc(1, sizeof(char*));
    names[0]=(const char *)"GetKeyString";
    return names;
}

int GetNumVariables(void){
    return 0;
}

void ** GetVariables(void){
    return NULL;
}

const char ** GetVariableNames(void){
    return NULL;
}

v8::Handle<v8::Value> GetKeyString(const v8::Arguments& args){
	if(args.Length()<1){
        return v8::ThrowException(v8::String::New("GetKeyString Error: Called with no arguments"));
	}
	CHECK_ARG_INT(0, "GetKeyString Error: Argument 0 is not a number.");

    bool shift = false;

	if(args.Length()>1){
        CHECK_ARG_BOOL(1, "GetKeyString Error: Argument 1 is not a boolean.");
        if(args[1]->v8::Value::BooleanValue()){
            shift = true;
        }
	}

    unsigned int keynum = args[0]->v8::Value::Int32Value();

    if(shift){
        keynum = toupper(keynum);
    }

    std::string key(1, keynum);
    return v8::String::New(key.c_str());
}
