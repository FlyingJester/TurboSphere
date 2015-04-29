#include "script.hpp"
#include "platform/time.h"

namespace Chrono {
    
    bool Delay(JSContext *ctx, unsigned argc, JS::Value *vp){
        JS::CallArgs args = CallArgsFromVp(argc, vp);
        const Turbo::JSType signature[] = {Turbo::Number};
        
        static unsigned long long last_gc_second = TS_Chrono_GetTime(TS_Chrono_Millisecond)/1000;
        
        if(!Turbo::CheckSignature<1>(ctx, args, signature, __func__))
            return false;
        
        long long nanoseconds = args[0].toNumber()*1000000.0;
        if(nanoseconds==0ll)
            return true;
        
        const unsigned long long c_second = TS_Chrono_GetTime(TS_Chrono_Millisecond)/1000;
        
        if((last_gc_second!=c_second) && nanoseconds>10000ull){
            unsigned long long gc_diff = TS_Chrono_GetTime(TS_Chrono_Nanosecond);
            
            if(nanoseconds>1000000ull){
                JS_GC(JS_GetRuntime(ctx));
            }
            else{
                JS_MaybeGC(ctx);
            }
            
            last_gc_second = c_second;
            nanoseconds-=(TS_Chrono_GetTime(TS_Chrono_Nanosecond)-gc_diff);
        }
        
        if(nanoseconds){
            TS_Chrono_Sleep(ctx, nanoseconds, TS_Chrono_Nanosecond);
        }
        
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
