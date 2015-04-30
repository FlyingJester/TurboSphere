#pragma once

#include <jsapi.h>

class regVariable {
    
    std::string name;
    
    regVariable(){}
public:
    regVariable(const std::string &name_, JS::Value val, JSContext *ctx);
    ~regVariable();
    void registerToContext(JSContext *) const;
};
