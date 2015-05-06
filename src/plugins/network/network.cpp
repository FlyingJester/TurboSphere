#include "network.hpp"
#include "script.hpp"
#include <array>
#include <cassert>

static std::array<JSNative, 4> function_list = {{
    Network::GetLocalName, 
    Network::GetLocalAddress, 
    Network::OpenAddress,
    Network::ListenOnPort
}};

static std::array<const char * const, 4> function_name_list = {{
    "GetLocalName",
    "GetLocalAddress",
    "OpenAddress",
    "ListenOnPort"
}};

static JSFunctionSpec listening_socket_methods[] = {
    JS_FN("accept", Network::Accept, 0, 0),
    JS_FN("close", Network::ListeningSocketClose, 0, 0),
    JS_FS_END
};


static JSFunctionSpec socket_methods[] = {
    JS_FN("isConnected", Network::SocketIsConnected, 0, 0),
    JS_FN("getPendingReadSize", Network::SocketGetPendingReadSize, 0, 0),
    JS_FN("write", Network::SocketWrite, 1, 0),
    JS_FN("read", Network::SocketRead, 1, 0),
    JS_FN("close", Network::SocketClose, 0, 0),
    JS_FS_END
};

const char *Init(JSContext *ctx, unsigned ID){
    assert(ctx);
    assert(function_name_list.size() == function_list.size());
    
    Network::socket_proto.initForContext(ctx, nullptr, socket_methods);
    Network::listening_socket_proto.initForContext(ctx, nullptr, listening_socket_methods);
    
    return PLUGINNAME;
}

void Close(JSContext *ctx){
    assert(ctx);
    Network::socket_proto.closeForContext(ctx);
    Network::listening_socket_proto.closeForContext(ctx);
}
    
int NumFunctions(JSContext *ctx){
    assert(ctx);
    return function_name_list.size();
}

JSNative GetFunction(JSContext *ctx, int n){
    assert(ctx);
    assert(n>=0);
    assert(n<function_list.size());
    
    return function_list[n];
}

const char * GetFunctionName(JSContext *ctx, int n){
    assert(ctx);
    assert(n>=0);
    assert(n<function_name_list.size());
    
    return function_name_list[n];
}
    
int NumVariables(JSContext *ctx){
    assert(ctx);
    return 0;
}

void GetVariable(JSContext *ctx, int n, JS::MutableHandleValue val){
    assert(ctx);
    assert(n>=0);
    assert(false);
}

const char * GetVariableName(JSContext *ctx, int n){
    assert(ctx);
    assert(n>=0);
    assert(false);
    return nullptr;
}
