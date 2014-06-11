#ifndef SAPPHIRE_SCREEN_HEAD
#define SAPPHIRE_SCREEN_HEAD

#include "sapphire.h"
#include "../common/plugin.h"

namespace Sapphire {
    namespace Script {
        Turbo::JSFunction GetScreenWidth(   Turbo::JSArguments);
        Turbo::JSFunction GetScreenHeight(  Turbo::JSArguments);

//        Turbo::JSFunction GetClippingRectangle(Turbo::JSArguments);
//        Turbo::JSFunction SetClippingRectangle(Turbo::JSArguments);
    }
}

#endif
