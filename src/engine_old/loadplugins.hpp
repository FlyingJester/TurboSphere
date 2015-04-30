#pragma once
#include <TSPR/dlopenwrap.h>
#include <jsapi.h>

void loadAllPlugins(JSContext *ctx);
void closeAllPlugins(JSContext *ctx);

void grabFuncsFromLibrary(fhandle handle);

// For use with regFunction and regVariable containers.
template<class T>
void registerAll(const T &items, JSContext *ctx){
    for(typename T::const_iterator iter = items.cbegin(); iter!=items.cend(); iter++){
        iter->registerToContext(ctx);
    }
}
