#include "SapphirePlugin.hpp"
#include "SapphireInit.hpp"
#include "Sapphire.hpp"
#include "script.hpp"

const char * Init(int aID){

    Sapphire::Script::InitScript(aID);

    Sapphire::Init(aID);

    return PLUGINNAME;
}

void Close(void){

}

using namespace Sapphire;

int GetNumFunctions(void){
    return Script::NumFuncs;
}

const Turbo::JSFunctionArray GetFunctions(void){
    return Script::FunctionList.data();
}

const Turbo::JSNameArray GetFunctionNames(void){
    return Script::FunctionNameList.data();
}

int GetNumVariables(void){
    return Script::NumVars;
}

const Turbo::JSValueArray GetVariables(void){
    return Script::VariableList.data();
}

const Turbo::JSNameArray GetVariableNames(void){
    return Script::VariableNameList.data();
}
