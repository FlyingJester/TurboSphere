#include "variableregister.h"

static std::vector<regVariable> varlist;

regVariable::regVariable(const char *_name, v8::Handle<v8::Value> _data){
    name = _name;
    data = _data;
}
regVariable::~regVariable(){

}

void regVariable::registerToContext(v8::Persistent<v8::Context> context){
    context->Global()->Set(v8::String::New(name), data);
}

void addVariableToList(const char *name, v8::Handle<v8::Value> data){
    varlist.push_back(regVariable(name, data));
}

void registerAllVariables(v8::Persistent<v8::Context> context){
    for(size_t i = 0; i<varlist.size(); i++){
        varlist[i].registerToContext(context);
    }
}
