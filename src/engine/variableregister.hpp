#ifndef VARIABLEREGISTER_HEAD
#define VARIABLEREGISTER_HEAD

#include "v8.h"

#include <vector>

typedef v8::Handle<v8::Value> *VariableArray;

class regVariable {
public:
    regVariable(const char *, v8::Handle<v8::Value>, v8::Isolate *iso);
    ~regVariable();
    void registerToContext(v8::Handle<v8::Context> context, v8::Isolate *iso) const;
    const char *name;
    v8::Handle<v8::Value> data;
};

void addVariableToList(const char *name, v8::Handle<v8::Value> data, v8::Isolate *iso);

void registerAllVariables(v8::Handle<v8::Context> context, v8::Isolate *iso);

#endif
