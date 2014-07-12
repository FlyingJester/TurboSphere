#include "SapphirePlugin.hpp"
#include "Sapphire.hpp"
#include "script.hpp"

using namespace Sapphire;

const char * Init(int aID){

    Script::InitScript(aID);

    return PLUGINNAME;
}

void Close(void){

}

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
