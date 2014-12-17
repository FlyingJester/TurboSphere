#include <string>
#include <vector>
#include <cstring>
#include "engine.h"
#include "functionload.h"

#ifdef _MSC_VER
	#define STRDUP _strdup
#else
	#define STRDUP strdup
#endif

static std::vector<regFunction> funclist;

regFunction::regFunction(const char *name_, v8::Handle<v8::FunctionTemplate> templ_, v8::Isolate *iso)
  : name(name_)
  , templ(templ_){

}

int registerfunction(const char *name, void (*func)(const v8::FunctionCallbackInfo<v8::Value> &args), v8::Isolate *iso){
    if(!func){
        return 1;
    }
    if(name==nullptr){
        return 1;
    }
    regFunction newfunc(name, v8::FunctionTemplate::New(iso, *func), iso);
    funclist.push_back(newfunc);
    return 0;
}

regFunction::regFunction(const char *_name, void (*func)(const v8::FunctionCallbackInfo<v8::Value> &args), v8::Isolate *iso){
    name    = _name;
    templ   = v8::FunctionTemplate::New(iso, *func);
}

regFunction::~regFunction(){
    //free((void*)name);
}

void regFunction::registerToContext(v8::Handle<v8::Context> context, v8::Isolate *iso){
    printf("[Engine] Info: Name is at %p\tname is %s\n", name, name);
    context->Global()->Set(v8::String::NewFromUtf8(iso, name), templ->GetFunction());
}


int registerFuncsFromDL(int numfuncs, void (*funcs[])(const v8::FunctionCallbackInfo<v8::Value> &args), const char * names[], v8::Handle<v8::Context> context){

    for(int i = 0; i<numfuncs; i++){
        regFunction *tempfunc = new regFunction(names[i], funcs[i], v8::Isolate::GetCurrent());
        tempfunc->registerToContext(context, v8::Isolate::GetCurrent());
    }
    return numfuncs;
}

void addFunctionToList(const char *name, void (*func)(const v8::FunctionCallbackInfo<v8::Value> &args), v8::Isolate *iso){
    if(name==nullptr){
        printf("Problem with plugin %s.\n", name);
    }
    regFunction r(name, func, iso);
    funclist.push_back(r);
}

void registerAllFunctions(v8::Handle<v8::Context> context, v8::Isolate *iso){
    for(size_t i = 0; i<funclist.size(); i++){
        //printf("Registering function %s to context.\n", funclist[i].name);
        funclist[i].registerToContext(context, iso);
    }
}

//void grabFuncsFromLibrary(library_handle, filename)


