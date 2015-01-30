#pragma once
#include <string>
#include <vector>
#include <jsapi.h>

namespace Turbo{
    
    typedef const char *(*InitFunction)(JSContext *ctx, unsigned ID);
    typedef void (*CloseFunction)(JSContext *ctx);
    typedef int (*NumFunction)(JSContext *ctx);
    typedef JSNative (*GetFuncFunction)(JSContext *ctx, int n);
    typedef JS::Heap<JS::Value> *(*GetVarFunction)(JSContext *ctx, int n);
    typedef const char *(*GetNameFunction)(JSContext *ctx, int n);
    
    bool loadAllPlugins(JSContext *ctx, const std::string &dir);
    bool listAllPlugins(std::vector<const std::string> &those, const std::string &dir);
    bool loadPlugin(JSContext *ctx, const std::string &path);

}
