#include "SapphirePlugin.hpp"
#include "SapphireInit.hpp"
#include "Sapphire.hpp"
#include "script.hpp"


#define L_WINDOW_TEST 1

#ifdef L_WINDOW_TEST
#include <unistd.h>
#include <cstdlib>
#include "GLStart.hpp"

const char *L_NUMS[] = {
  "one",
  "two",
  "three",
  "four",
  "five"
};

#endif


const char * Init(int aID){

    Sapphire::Script::InitScript(aID);

    Sapphire::Init(aID);

    #ifdef L_WINDOW_TEST
    for(int i = 0; i<5; i++){
        printf(BRACKNAME "Test: That's %s.\n", L_NUMS[i]);
        sleep(1);
    }

    Sapphire::GL::RenderThread::StopThread();

    exit(EXIT_SUCCESS);

    #endif

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
