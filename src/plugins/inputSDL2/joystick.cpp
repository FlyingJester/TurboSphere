#include "joystick.hpp"
#include <vector>
#include <algorithm>
#include <SDL2/SDL_joystick.h>
#include <jsapi.h>

namespace InputSDL2{
    
    std::vector<SDL_Joystick *> joysticks;
    unsigned refs = 0;
    
    void InitJoystick(JSContext *ctx){
        const unsigned num_joysticks = SDL_NumJoysticks();
        joysticks.resize(num_joysticks);
        
        for(unsigned i = 0; i<num_joysticks; i++) joysticks[i] = SDL_JoystickOpen(i);
        
        refs++;
    }
    
    void CloseJoystick(JSContext *ctx){
        if((--refs)==0){
            std::for_each(joysticks.begin(), joysticks.end(), SDL_JoystickClose);
            joysticks.clear();
        }
    }

    namespace Script{
        
        bool GetNumJoysticks(JSContext *ctx, unsigned argc, JS::Value *vp){
            CallArgsFromVp(argc, vp).rval().set(JS_NumberValue(joysticks.size()));
            return true;
        }
        
        bool GetJoystickName(JSContext *ctx, unsigned argc, JS::Value *vp){
            JS::CallArgs args = CallArgsFromVp(argc, vp);
            if(!Turbo::CheckForSingleArg(ctx, args, Turbo::Number, __func__))
                return false;
            
            SDL_Joystick * const js = joysticks[args[0].toNumber()];
            const char * const name = SDL_JoystickName(js);
            
            args.rval().set(STRING_TO_JSVAL(JS_NewStringCopyZ(ctx, name)));
            return true;
        }
        
        bool GetNumJoystickButtons(JSContext *ctx, unsigned argc, JS::Value *vp){
            JS::CallArgs args = CallArgsFromVp(argc, vp);
            if(!Turbo::CheckForSingleArg(ctx, args, Turbo::Number, __func__))
                return false;
            
            SDL_Joystick * const js = joysticks[args[0].toNumber()];
            const unsigned num_buttons = SDL_JoystickNumButtons(js);
            
            args.rval().set(JS_NumberValue(num_buttons));
            return true;
        }
        
        bool GetNumJoystickAxes(JSContext *ctx, unsigned argc, JS::Value *vp){
            JS::CallArgs args = CallArgsFromVp(argc, vp);
            if(!Turbo::CheckForSingleArg(ctx, args, Turbo::Number, __func__))
                return false;
            
            SDL_Joystick * const js = joysticks[args[0].toNumber()];
            const unsigned num_buttons = SDL_JoystickNumAxes(js);
            
            args.rval().set(JS_NumberValue(num_buttons));
            return true;
        }
        
        bool IsJoystickButtonPressed(JSContext *ctx, unsigned argc, JS::Value *vp){
            JS::CallArgs args = CallArgsFromVp(argc, vp);
            const enum Turbo::JSType signature[] = {Turbo::Number, Turbo::Number};
            
            if(!Turbo::CheckSignature<2>(ctx, args, signature, __func__))
                return false;
            
            SDL_Joystick * const js = joysticks[args[0].toNumber()];
            const int button = args[1].toNumber();
            if((button<0) || (button>=SDL_JoystickNumButtons(js))){
                Turbo::SetError(ctx, std::string(BRACKNAME " Error argument 0 is out of range"));
                return false;
            }
            
            args.rval().set(BOOLEAN_TO_JSVAL(SDL_JoystickGetButton(js, button)));
            return true;
        }
        
        bool GetJoystickAxis(JSContext *ctx, unsigned argc, JS::Value *vp){
            JS::CallArgs args = CallArgsFromVp(argc, vp);
            const enum Turbo::JSType signature[] = {Turbo::Number, Turbo::Number};
            
            if(!Turbo::CheckSignature<2>(ctx, args, signature, __func__))
                return false;
            
            SDL_Joystick * const js = joysticks[args[0].toNumber()];
            const int axis = args[1].toNumber();
            
            if((axis<0) || (axis>=SDL_JoystickNumAxes(js))){
                Turbo::SetError(ctx, std::string(BRACKNAME " Error argument 0 is out of range"));
                return false;
            }
            
            args.rval().set(JS_NumberValue(SDL_JoystickGetAxis(js, axis)));
            return true;
        }
    }
}
