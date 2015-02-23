#pragma once
#include <string>
#include <vector>
#include <jsapi.h>

namespace Turbo{
    
    typedef const char *(*InitFunction)(JSContext *ctx, unsigned ID);
    typedef void (*CloseFunction)(JSContext *ctx);
    typedef int (*NumFunction)(JSContext *ctx);
    typedef JSNative (*GetFuncFunction)(JSContext *ctx, int n);
    typedef void (*GetVarFunction)(JSContext *ctx, int n, JS::MutableHandleValue value);
    typedef const char *(*GetNameFunction)(JSContext *ctx, int n);
    
    struct Plugin{
        const char *name;
        unsigned num_functions;
        unsigned num_variables;
        
        void *handle;
        
        struct {
            InitFunction Init;
            CloseFunction Close;
            NumFunction NumFunctions;
            GetFuncFunction GetFunction;
            GetNameFunction GetFunctionName;
            
            NumFunction NumVariables;
            GetVarFunction GetVariable;
            GetNameFunction GetVariableName;
            
        } API;
    };
    
    // Also initializes
    bool loadAllPlugins(JSContext *ctx, std::vector<struct Plugin> &those, const std::string &dir);
    bool listAllPlugins(std::vector<const std::string> &those, const std::string &dir);
    bool loadPlugin(JSContext *ctx, struct Plugin &to, const std::string &path);
    bool closeAllPlugins(JSContext *ctx, std::vector<struct Plugin> &those);
    
    
}

