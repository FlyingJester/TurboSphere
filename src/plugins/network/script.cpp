#include "script.hpp"
#include "gethostaddress.h"
    
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <ifaddrs.h>
#endif

namespace NetPlug{

std::vector<struct ListeningPort> listen_list;

void SocketFinalizer(JSFreeOp *fop, JSObject *obj){
    
    struct Socket *mSocket = socket_proto.unsafeUnwrap(obj);
    if(!mSocket){
        puts(BRACKNAME " SocketFinalizer Warning private value was NULL");
        return;
    }
    
    if(Internal::SocketIsConnected(mSocket))
        Disconnect_Socket(mSocket->socket);
    
    if(mSocket->socket)
        Destroy_Socket(mSocket->socket);
   
    free(mSocket);
   
}

Turbo::JSPrototype<struct Socket> socket_proto("Socket", nullptr, 0, SocketFinalizer);

bool GetLocalName(JSContext *ctx, unsigned argc, JS::Value *vp){
    char name[2048];
    
    gethostname(name, 2048);
    name[2047] = '\0';
    
    CallArgsFromVp(argc, vp).rval().set(STRING_TO_JSVAL(JS_NewStringCopyZ(ctx, name)));
    return true;
}

bool GetLocalAddress(JSContext *ctx, unsigned argc, JS::Value *vp){
    const char *addr = TS_GetHostAddressIP4();
    if(!addr)
        addr = TS_GetHostAddressIP6();
    if(!addr)
        addr = "";
    
    CallArgsFromVp(argc, vp).rval().set(STRING_TO_JSVAL(JS_NewStringCopyZ(ctx, addr)));
    return true;
}

bool OpenAddress(JSContext *ctx, unsigned argc, JS::Value *vp){
    
    const Turbo::JSType signature[] = {Turbo::String, Turbo::Number};
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<2>(ctx, args, signature, __func__))
        return false;
    
    struct Socket *mSocket = static_cast<struct Socket *>(malloc(sizeof(struct Socket)));
    mSocket->socket = Create_Socket();
    mSocket->listening = false;
    
    struct Turbo::JSStringHolder<> address(ctx, JS_EncodeString(ctx, args[0].toString()));
    
    enum WSockErr err = Connect_Socket(mSocket->socket, address.string, args[1].toNumber(), -1);
    if(err!=0){
        Turbo::SetError(ctx, std::string(BRACKNAME " OpenAddress Error ") + ExplainError_Socket(err));
        return false;
    }
    
    args.rval().set(OBJECT_TO_JSVAL(socket_proto.wrap(ctx, mSocket)));
    return true;
}

bool ListenOnPort(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    if(!Turbo::CheckForSingleArg(ctx, args, Turbo::Number, __func__))
        return false;
    
    const long port = args[0].toNumber();
    
    struct ListeningPort *accept = nullptr;
    
    for(std::vector<struct ListeningPort>::iterator iter = listen_list.begin(); iter!=listen_list.end(); iter++){
        if(iter->port==port){
            accept = &(*iter);
            break;
        }
    }
    
    if(accept==nullptr){
        struct WSocket *listening_socket = Create_Socket();
        Listen_Socket(listening_socket, port);
        
        listen_list.push_back({0ul, port, listening_socket});
        accept = &listen_list.back();
    }
    
    
    struct Socket *mSocket = static_cast<struct Socket *>(malloc(sizeof(struct Socket)));
    mSocket->listening = true;
    mSocket->connected = false;
    mSocket->port = port;
    mSocket->accept = accept;
    mSocket->socket = nullptr;
    
    accept->references++;
    
    args.rval().set(OBJECT_TO_JSVAL(socket_proto.wrap(ctx, mSocket)));
        
    return true;
}

bool Internal::SocketIsConnected(struct Socket *aSocket){
    if(aSocket->listening){
        if(aSocket->connected){
            return State_Socket(aSocket->socket)!=0;
        }
        else if(aSocket->accept){
            aSocket->socket = Accept_Socket(aSocket->accept->socket);
            if(!aSocket->socket){
                return false;
            }
            else{
                aSocket->accept = nullptr;
                aSocket->connected = true;
                return true;
            }
        }
        else{
            return false;
        }
    }
    else{
        return State_Socket(aSocket->socket)!=0;
    }
}
    
bool SocketIsConnected(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    args.rval().set(BOOLEAN_TO_JSVAL(Internal::SocketIsConnected(socket_proto.getSelf(ctx, vp, &args))));
    
    return true;
}

bool SocketGetPendingReadSize(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    struct Socket *mSocket = socket_proto.getSelf(ctx, vp, &args);
    
    if(!Internal::SocketIsConnected(mSocket))
        args.rval().set(JS_NumberValue(0));
    else
        args.rval().set(JS_NumberValue((Length_Socket(mSocket->socket)==0)));
    return true;
}

bool SocketWrite(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    if(!Turbo::CheckForSingleArg(ctx, args, Turbo::ArrayBuffer, __func__))
        return false;
    
    struct Socket *mSocket = socket_proto.getSelf(ctx, vp, &args);
    
    if(!Internal::SocketIsConnected(mSocket)){
        Turbo::SetError(ctx, std::string(BRACKNAME " SocketWrite Error socket not connected"));
        return false;
    }
    
    JS::RootedObject buffer_root(ctx, args[0].toObjectOrNull());
    
    void *data;
    uint32_t len;
    
    js::GetArrayBufferLengthAndData(buffer_root, &len, (unsigned char **)(&data));
    
    enum WSockErr err = Write_Socket_Len(mSocket->socket, (const char *)data, len);
    if(err!=0){
        Turbo::SetError(ctx, std::string(BRACKNAME " SocketWrite Error ") + ExplainError_Socket(err));
        return false;
    }
    
    return true;
}

bool SocketRead(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);    
    
    struct Socket *mSocket = socket_proto.getSelf(ctx, vp, &args);
    
    if(!Internal::SocketIsConnected(mSocket)){
        Turbo::SetError(ctx, std::string(BRACKNAME " SocketRead Error socket not connected"));
        return false;
    }
    
    char *data = NULL;
    enum WSockErr err = Read_Socket(mSocket->socket, &data);
        if(err!=0){
        Turbo::SetError(ctx, std::string(BRACKNAME " SocketRead Error ") + ExplainError_Socket(err));
        return false;
    }
    
    JSObject *buffer = JS_NewArrayBufferWithContents(ctx, strlen(data), data);
    
    args.rval().set(OBJECT_TO_JSVAL(buffer));
    return true;
}

bool SocketClose(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    struct Socket *mSocket = socket_proto.getSelf(ctx, vp, &args);
    if(Internal::SocketIsConnected(mSocket))
        Disconnect_Socket(mSocket->socket);
        
    return true;
}

    
}
