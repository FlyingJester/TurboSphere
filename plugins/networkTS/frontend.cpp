#include "frontend.h"
#include "networkTS.h"

v8Function GetHostName(V8ARGS){
    static const char * cstr = TS_GetHostName();
    auto str = v8::String::New((cstr!=NULL)?cstr:"");
    //TS_FreeHostName(cstr);
    return str;
}
