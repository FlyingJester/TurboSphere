#include "engine.h"
#include "variableregister.h"

static std::vector<regVariable> varlist;

regVariable::regVariable(const char *_name, v8::Handle<v8::Value> _data, v8::Isolate *iso){
    name = _name;
    data = _data;
}
regVariable::~regVariable(){

}

void regVariable::registerToContext(v8::Handle<v8::Context> context, v8::Isolate *iso){
    context->Global()->Set(v8::String::NewFromUtf8(iso, name), data);
}

void addVariableToList(const char *name, v8::Handle<v8::Value> data, v8::Isolate *iso){
    varlist.push_back(regVariable(name, data, iso));
}

void registerAllVariables(v8::Handle<v8::Context> context, v8::Isolate *iso){
    for(size_t i = 0; i<varlist.size(); i++){
        varlist[i].registerToContext(context, iso);
    }
}
