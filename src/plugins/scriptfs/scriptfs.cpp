
#include <array>
#include <t5.h>
#include "scriptfs.hpp"
#include <openscript.h>
#include <opengame.h>
#include "script.hpp"
#include "rawfile.hpp"

namespace scriptfs {

    std::array<Turbo::JSCallback, NUM_FUNCS> Functions = {{
        scriptfs::OpenRawFile,
        T5Call<bool, t5::IsFile>,
        T5Call<bool, t5::IsDir>
    }};

    std::array<Turbo::JSName, NUM_FUNCS> FunctionNames = {{
        "RawFile",
        "IsFile",
        "IsDirectory",
    }};

    std::array<Turbo::JSValue, NUM_VARS> Variables = {{
        v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), GetDirs()->root),
        v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), GetDirs()->system)
    }};
    std::array<Turbo::JSName, NUM_VARS> VariableNames = {{
        "FILESYSTEM_ROOT",
        "FILESYSTEM_SYSTEM"
    }};

}

void Close(void){

}

const char * Init(int ID){
    InitRawFile(ID);

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
    return scriptfs::Variables.data();

}

Turbo::JSNameArray GetVariableNames(void){
    return scriptfs::VariableNames.data();
}
