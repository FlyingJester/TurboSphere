#include <string>
#include <vector>
#include <cstring>
#include "engine.hpp"
#include "functionload.hpp"

#ifdef _MSC_VER
	#define STRDUP _strdup
#else
	#define STRDUP strdup
#endif

static std::vector<regFunction> funclist;

regFunction::regFunction(const std::string &name_, v8::Handle<v8::FunctionTemplate> templ_, v8::Isolate *iso)
  : name(name_)
  , templ(templ_){

}

regFunction::regFunction(const std::string &name_, void (*func)(const v8::FunctionCallbackInfo<v8::Value> &args), v8::Isolate *iso){
    name    = name_;
    templ   = v8::FunctionTemplate::New(iso, *func);
}

void regFunction::registerToContext(v8::Handle<v8::Context> context, v8::Isolate *iso) const{
    printf("[Engine] Info: Name is %s\n", name.c_str());
    context->Global()->Set(v8::String::NewFromUtf8(iso, name.c_str()), templ->GetFunction());
}


int registerFuncsFromDL(int numfuncs, void (*funcs[])(const v8::FunctionCallbackInfo<v8::Value> &args), const char * names[], v8::Handle<v8::Context> context){

    for(int i = 0; i<numfuncs; i++){
        regFunction tempfunc(names[i], funcs[i], v8::Isolate::GetCurrent());
        tempfunc.registerToContext(context, v8::Isolate::GetCurrent());
    }
    return numfuncs;
}

void addFunctionToList(const std::string &name, void (*func)(const v8::FunctionCallbackInfo<v8::Value> &args), v8::Isolate *iso){
    funclist.push_back(regFunction(name, func, iso));
}

void registerAllFunctions(v8::Handle<v8::Context> context, v8::Isolate *iso){
    for(std::vector<regFunction>::const_iterator i = funclist.cbegin(); i!=funclist.cend(); i++){
        i->registerToContext(context, iso);
    }
}



