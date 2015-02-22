#pragma once
#include "network.hpp"
#include "libfjnet/socket.h"
#include <vector>

namespace NetPlug{
    
    /*
        listening is true if this is socket opened by ListenOnPort
        
        if listening is true:
            
            port is the port we are listening on.
            
            accept is a point to the listen_list entry that we will
                accept a socket from.
            
            socket will be nullptr if we have not accepted yet.
            
            accept will be nullptr if we have connected.
            
            (last two points rephrased):
                if connected is false:
                    accept is not NULL, points to socket to Accept_Socket from
                    socket is NULL, will be filled with Accept_Socket
                if connected is true:
                    accept is NULL
                    socket is valid to use, not NULL
            
        connected is true if we have connected.
        
        in all cases, socket may be NULL if connected is false
        
        socket will never be NULL if connected is true.
        
    */
    
    struct Socket{
        bool listening;
        bool connected;
        long port;
        struct ListeningPort *accept;
        struct WSocket *socket;
    };
    
    struct ListeningPort {
        unsigned long references;
        long port;
        struct WSocket *socket;
    };
    
    namespace Internal{
        bool SocketIsConnected(struct Socket *socket);
    }
    
    extern std::vector<struct ListeningPort> listen_list;
    extern Turbo::JSPrototype<struct Socket> socket_proto;
    
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
