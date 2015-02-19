#pragma once
#include "network.hpp"

#include "libfjnet/socket.h"

namespace NetPlug{
    
    extern Turbo::JSPrototype<struct WSocket> socket_proto;
    
    bool GetLocalName(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool GetLocalAddress(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool OpenAddress(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool ListenOnPort(JSContext *ctx, unsigned argc, JS::Value *vp);
    
    bool SocketIsConnected(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool SocketGetPendingReadSize(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool SocketWrite(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool SocketRead(JSContext *ctx, unsigned argc, JS::Value *vp);
    bool SocketClose(JSContext *ctx, unsigned argc, JS::Value *vp);
    
}
