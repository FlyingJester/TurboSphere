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

const char *Init(JSContext *ctx, unsigned aID){

    Sapphire::Script::initScript(ctx);

    Sapphire::Init(ctx, aID);

    return PLUGINNAME;
}

void Close(JSContext *ctx){
    assert(ctx);
    Sapphire::Script::closeScript(ctx);
}

int NumFunctions(JSContext *ctx){
    assert(ctx);
    return Sapphire::Script::FunctionList.size();
}

JSNative GetFunction(JSContext *ctx, int n){
    assert(ctx);
    assert(n<Sapphire::Script::FunctionList.size());
    
    return Sapphire::Script::FunctionList[n];
}

const char * GetFunctionName(JSContext *ctx, int n){
    assert(ctx);
    assert(n<Sapphire::Script::FunctionNameList.size());
    
    return Sapphire::Script::FunctionNameList[n];
}

int  NumVariables(JSContext *ctx){
    assert(ctx);
    return 0;
}
JS::Heap<JS::Value> * GetVariable(JSContext *ctx, int n){
    assert(ctx);
    assert(false);
    
    return nullptr;
}
const char *  GetVariableName(JSContext *ctx, int n){
    assert(ctx);
    assert(false);
    
    return nullptr;
}
