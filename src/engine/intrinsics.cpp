#include <cassert>
#include <openscript.h>
#include "intrinsics.hpp"

bool Turbo::initializeIntrinsicFunctions(JSContext *ctx, JS::HandleObject global){
    if(!ctx)
        return false;
    
    return 
        JS_DefineFunction(ctx, global, "RequireScript", &TS_LoadScript_JS<true>, 1, 0) &&
        JS_DefineFunction(ctx, global, "RequireSystemScript", &TS_LoadSystemScript_JS<true>, 1, 0)&&
        JS_DefineFunction(ctx, global, "EvaluateScript", &TS_LoadScript_JS<false>, 1, 0) &&
        JS_DefineFunction(ctx, global, "EvaluateSystemScript", &TS_LoadSystemScript_JS<false>, 1, 0);

}
