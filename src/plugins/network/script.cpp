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

namespace Network{

void SocketFinalizer(JSFreeOp *fop, JSObject *obj){
    
    struct WSocket *mSocket = socket_proto.unsafeUnwrap(obj);
    if(!mSocket){
        puts(BRACKNAME " SocketFinalizer Warning private value was NULL");
        return;
    }

    if(State_Socket(mSocket))
        Disconnect_Socket(mSocket);

    //Destroy_Socket(mSocket);
   
}
void ListeningSocketFinalizer(JSFreeOp *fop, JSObject *obj){
    
    struct WSocket *mSocket = listening_socket_proto.unsafeUnwrap(obj);
    if(!mSocket){
        puts(BRACKNAME " ListeningSocketFinalizer Warning private value was NULL");
        return;
    }

    //Destroy_Socket(mSocket);

}
Turbo::JSPrototype<struct WSocket> socket_proto("Socket", OpenAddress, 2, SocketFinalizer);
Turbo::JSPrototype<struct WSocket> listening_socket_proto("ListeningSocket", ListenOnPort, 1, ListeningSocketFinalizer);

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
    
    struct WSocket *mSocket = Create_Socket();
    
    struct Turbo::JSStringHolder<> address(ctx, JS_EncodeString(ctx, args[0].toString()));
    
    enum WSockErr err = Connect_Socket(mSocket, address.string, args[1].toNumber(), -1);
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

    struct WSocket *mSocket = Create_Socket();
    
    enum WSockErr err = Listen_Socket(mSocket, port);
    
    if(err!=0){
        Turbo::SetError(ctx, std::string(BRACKNAME " ListenOnPort Error ") + ExplainError_Socket(err));
        return false;
    }
    
    args.rval().set(OBJECT_TO_JSVAL(listening_socket_proto.wrap(ctx, mSocket)));
        
    return true;

}

bool Accept(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
        
    struct WSocket *socket = Accept_Socket(listening_socket_proto.getSelf(ctx, vp, &args));
    
    if(!socket){
        args.rval().set(JSVAL_NULL);
        return true;
    }
    
    args.rval().set(OBJECT_TO_JSVAL(socket_proto.wrap(ctx, socket)));
    return true;

}

bool ListeningSocketClose(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    struct WSocket *mSocket = listening_socket_proto.getSelf(ctx, vp, &args);
    if(State_Socket(mSocket)==0)
        Disconnect_Socket(mSocket);
        
    return true;
}

bool SocketIsConnected(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    struct WSocket *socket = socket_proto.getSelf(ctx, vp, &args);
    
    if(!socket){
        return false;
    }
    
    args.rval().set(BOOLEAN_TO_JSVAL(State_Socket(socket)));
    
    return true;
}

bool SocketGetPendingReadSize(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    struct WSocket *mSocket = socket_proto.getSelf(ctx, vp, &args);
    
    if(State_Socket(mSocket)!=0)
        args.rval().set(JS_NumberValue(0));
    else
        args.rval().set(JS_NumberValue((Length_Socket(mSocket))));
    return true;
}

bool SocketWrite(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    if(!Turbo::CheckForSingleArg(ctx, args, Turbo::ArrayBuffer, __func__))
        return false;
    
    struct WSocket *mSocket = socket_proto.getSelf(ctx, vp, &args);
    
    if(State_Socket(mSocket)!=0){
        Turbo::SetError(ctx, std::string(BRACKNAME " SocketWrite Error socket not connected"));
        return false;
    }
    
    JS::RootedObject buffer_root(ctx, args[0].toObjectOrNull());
    
    void *data;
    uint32_t len;
    
    js::GetArrayBufferLengthAndData(buffer_root, &len, (unsigned char **)(&data));
    
    enum WSockErr err = Write_Socket_Len(mSocket, (const char *)data, len);
    if(err!=0){
        Turbo::SetError(ctx, std::string(BRACKNAME " SocketWrite Error ") + ExplainError_Socket(err));
        return false;
    }
    
    return true;
}

bool SocketRead(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);    
    
    struct WSocket *mSocket = socket_proto.getSelf(ctx, vp, &args);
    
    if(State_Socket(mSocket)!=0){
        Turbo::SetError(ctx, std::string(BRACKNAME " SocketRead Error socket not connected"));
        return false;
    }
    
    char *data = NULL;
    enum WSockErr err = Read_Socket(mSocket, &data);
        if(err!=0){
        Turbo::SetError(ctx, std::string(BRACKNAME " SocketRead Error ") + ExplainError_Socket(err));
        return false;
    }
    
    JS::RootedObject buffer(ctx, JS_NewArrayBufferWithContents(ctx, strlen(data), data));
    
    printf("Array Buffer read in is %i long.\n", (int)strlen(data));
    
    args.rval().set(OBJECT_TO_JSVAL(buffer));
    return true;
}

bool SocketClose(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    struct WSocket *mSocket = socket_proto.getSelf(ctx, vp, &args);
    if(State_Socket(mSocket)==0)
        Disconnect_Socket(mSocket);
        
    return true;
}

    
}
