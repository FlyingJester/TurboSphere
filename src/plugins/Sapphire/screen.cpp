#include "screen.hpp"

#include "../../graphiccommon/screen.h"

Turbo::JSFunction Sapphire::Script::GetScreenWidth(   Turbo::JSArguments args){
    args.GetReturnValue().Set( ::GetScreenWidth());

}

Turbo::JSFunction Sapphire::Script::GetScreenHeight(  Turbo::JSArguments args){
    args.GetReturnValue().Set( ::GetScreenHeight());
}
