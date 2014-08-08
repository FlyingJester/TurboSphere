#include "SapphirePlugin.hpp"
#include "SapphireInit.hpp"
#include "Sapphire.hpp"
#include "script.hpp"

#include <t5.h>
#include <cstdio>

t5::DataSource *StdOutSource = t5::DataSource::FromCFileHandle(stdout);
t5::DataSourcePusher *StdOut = StdOutSource->AsPusher();

t5::DataSource *StdErrSource = t5::DataSource::FromCFileHandle(stderr);
t5::DataSourcePusher *StdErr = StdErrSource->AsPusher();

int T5_StdOut_Write(const char *aToWrite){
  StdOut->WriteS(aToWrite);
  return 0;
}
int T5_StdErr_Write(const char *aToWrite){
  StdErr->WriteS(aToWrite);
  return 0;
}

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
