#include "script.hpp"
#include "platform/time.h"

namespace Chrono {
    
    bool Delay(JSContext *ctx, unsigned argc, JS::Value *vp){
        JS::CallArgs args = CallArgsFromVp(argc, vp);
        const Turbo::JSType signature[] = {Turbo::Number};
        if(!Turbo::CheckSignature<1>(ctx, args, signature, __func__))
            return false;
        
        double value = args[0].toNumber();
        unsigned long nanoseconds = value*1000000.0f;
        
        TS_Chrono_Sleep(ctx, nanoseconds, TS_Chrono_Nanosecond);
        
        return true;
    }
    
    bool GetTime(JSContext *ctx, unsigned argc, JS::Value *vp){
        CallArgsFromVp(argc, vp).rval().set(JS_NumberValue(TS_Chrono_GetTime(TS_Chrono_Millisecond)));
        return true;
    }
    
    bool GetSeconds(JSContext *ctx, unsigned argc, JS::Value *vp){
        CallArgsFromVp(argc, vp).rval().set(JS_NumberValue(TS_Chrono_GetTime(TS_Chrono_Nanosecond)/1000000000.0));
        return true;
    }
 
}
