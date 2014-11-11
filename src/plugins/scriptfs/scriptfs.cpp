#include "scriptfs.hpp"
#include "script.hpp"
#include "rawfile.hpp"
#include <openscript.h>
#include <opengame.h>

#include <array>

namespace scriptfs {

    std::array<Turbo::JSCallback, NUM_FUNCS> Functions = {
        {scriptfs::OpenRawFile},
    };

    std::array<Turbo::JSName, NUM_FUNCS> FunctionNames = {
        {"RawFile"},
    };

    #if !(NUM_VARS == 0)

        std::array<Turbo::JSValue, NUM_VARS> Variables;
        std::array<Turbo::JSName, NUM_VARS> VariableNames;

    #endif
}

void Close(void){

}

const char * Init(int ID){
    InitRawFile(ID);

    {
        TS_Directories *TS_dirs = GetDirs();
        std::string scriptpath = std::string(TS_dirs->systemscript).append("turbo/arraybuffer_bytearray.js");
        std::string ScriptStr = openfile(scriptpath.c_str());

        if(ScriptStr.empty()) {
            printf(BRACKNAME " Error: Could not load system script turbo/arraybuffer_bytearray.js\n");
        }
        if(!ExecuteString(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), ScriptStr.c_str()), v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "turbo/arraybuffer_bytearray.js"), v8::Isolate::GetCurrent(), true)) {
            printf(BRACKNAME " Error in turbo/arraybuffer_bytearray.js\n");
        }
    }

    return "ScriptFS";
}

int GetNumFunctions(void){
    return NUM_FUNCS;
}

Turbo::JSFunctionArray GetFunctions(void){
    return scriptfs::Functions.data();
}

Turbo::JSNameArray GetFunctionNames(void){
    return scriptfs::FunctionNames.data();
}

int GetNumVariables(void){
return NUM_VARS;
}

Turbo::JSValueArray GetVariables(void){
    #if NUM_VARS == 0
    return nullptr;
    #else
    return scriptfs::Variables.data();
    #endif

}

Turbo::JSNameArray GetVariableNames(void){
    #if NUM_VARS == 0
    return nullptr;
    #else
    return scriptfs::VariableNames.data();
    #endif
}
