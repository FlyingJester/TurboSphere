
#include "engine.h"
#include "functionload.h"
#include <cstring>
#include <string>
#include <vector>

#ifdef _MSC_VER
	#define STRDUP _strdup
#else
	#define STRDUP strdup
#endif

static std::vector<regFunction> funclist;

regFunction::regFunction(const char *_name, v8::Handle<v8::FunctionTemplate> _templ){
    name = name;
    templ = _templ;
}

int registerfunction(const char *name, v8::Handle<v8::Value> (*func)(const v8::Arguments& args)){
    if(!func){
        return 1;
    }
    if(name==NULL){
        return 1;
    }
    regFunction newfunc(name, v8::FunctionTemplate::New(*func));
    funclist.push_back(newfunc);
    return 0;
}

regFunction::regFunction(const char *_name, v8::Handle<v8::Value> (*func)(const v8::Arguments& args)){
    name    = _name;
    templ   = v8::FunctionTemplate::New(*func);
}

regFunction::~regFunction(){
    //free((void*)name);
}

void regFunction::registerToContext(v8::Persistent<v8::Context> context){
    context->Global()->Set(v8::String::New(name), templ->GetFunction());
}


int registerFuncsFromDL(int numfuncs, v8::Handle<v8::Value> (*funcs[])(const v8::Arguments& args), const char * names[], v8::Persistent<v8::Context> context){

    for(int i = 0; i<numfuncs; i++){
        regFunction *tempfunc = new regFunction(STRDUP(names[i]), funcs[i]);
        tempfunc->registerToContext(context);
    }
    return numfuncs;
}

void addFunctionToList(const char *name, v8::Handle<v8::Value> (*func)(const v8::Arguments& args)){
    if(name==NULL){
        printf("Problem with plugin %i.\n", name);
    }
    regFunction r(STRDUP(name), func);
    funclist.push_back(r);
}

void registerAllFunctions(v8::Persistent<v8::Context> context){
    for(size_t i = 0; i<funclist.size(); i++){
        //printf("Registering function %s to context.\n", funclist[i].name);
        funclist[i].registerToContext(context);
    }
}

//void grabFuncsFromLibrary(library_handle, filename)


