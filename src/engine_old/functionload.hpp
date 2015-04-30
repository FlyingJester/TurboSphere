#pragma once

#include <jsapi.h>

class regFunction {
    std::string name;
    
    regFunction(){}
public:
    regFunction(const std::string &name_, JSNative, JSContext *);
    ~regFunction(){}
    
    void registerToContext(JSContext *) const;
};

