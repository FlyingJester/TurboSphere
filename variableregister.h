#ifndef VARIABLEREGISTER_HEAD
#define VARIABLEREGISTER_HEAD

#include "v8.h"

#include <vector>

typedef v8::Handle<v8::Value> * VariableArray;

class regVariable {
public:
    regVariable(const char *, v8::Handle<v8::Value>);
    ~regVariable();
    void registerToContext(v8::Persistent<v8::Context> context);
    const char *name;
    v8::Handle<v8::Value> data;
};

void addVariableToList(const char *name, v8::Handle<v8::Value> data);

void registerAllVariables(v8::Persistent<v8::Context> context);

#endif
