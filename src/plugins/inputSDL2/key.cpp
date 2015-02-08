#include "input_main.hpp"
#include "key.hpp"
#include <array>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_events.h>

namespace InputSDL2 {

std::array<const char * const, NUM_KEYS> key_names = {{
    "KEY_ESCAPE",    
    "KEY_F1",   
    "KEY_F2",
    "KEY_F3",
    "KEY_F4",
    "KEY_F5",
    "KEY_F6",
    "KEY_F7",
    "KEY_F8",
    "KEY_F9",
    "KEY_F10",    
    "KEY_F11",
    "KEY_F12",
    "KEY_TILDE",
    "KEY_0", 
    "KEY_1", 
    "KEY_2", 
    "KEY_3", 
    "KEY_4", 
    "KEY_5", 
    "KEY_6", 
    "KEY_7", 
    "KEY_8", 
    "KEY_9", 
    "KEY_MINUS",
    "KEY_EQUALS",
    "KEY_BACKSPACE",
    "KEY_TAB",
    "KEY_A", 
    "KEY_B", 
    "KEY_C", 
    "KEY_D", 
    "KEY_E", 
    "KEY_F", 
    "KEY_G", 
    "KEY_H", 
    "KEY_I", 
    "KEY_J", 
    "KEY_K", 
    "KEY_L", 
    "KEY_M", 
    "KEY_N", 
    "KEY_O", 
    "KEY_P", 
    "KEY_Q", 
    "KEY_R", 
    "KEY_S", 
    "KEY_T", 
    "KEY_U", 
    "KEY_V", 
    "KEY_W", 
    "KEY_X", 
    "KEY_Y", 
    "KEY_Z", 
    "KEY_SHIFT",
    "KEY_RSHIFT",
    "KEY_CTRL",
    "KEY_RCTRL",
    "KEY_ALT",
    "KEY_LALT",
    "KEY_SPACE",
    "KEY_OPENBRACE",
    "KEY_CLOSEBRACE",
    "KEY_SEMICOLON",
    "KEY_APOSTROPHE",
    "KEY_COMMA",
    "KEY_PERIOD",
    "KEY_SLASH",
    "KEY_BACKSLASH",
    "KEY_ENTER",
    "KEY_RETURN", // This is not in 1.5
    "KEY_INSERT",
    "KEY_DELETE",
    "KEY_HOME",
    "KEY_END",
    "KEY_PAGEUP",
    "KEY_PAGEDOWN",
    "KEY_UP",
    "KEY_RIGHT",
    "KEY_DOWN",
    "KEY_LEFT",
    "KEY_NUM_0",
    "KEY_NUM_1",
    "KEY_NUM_2",
    "KEY_NUM_3",
    "KEY_NUM_4",
    "KEY_NUM_5",
    "KEY_NUM_6",
    "KEY_NUM_7",
    "KEY_NUM_8",
    "KEY_NUM_9",
    "KEY_CAPSLOCK",
    "KEY_NUMLOCK",
    "KEY_SCROLLOCK", // For compatibility with Sphere 1.x 
    "KEY_SCROLLLOCK",  
}};

std::array<unsigned, NUM_KEYS> key_numbers = {{ // Translates directly to an SDL key constant
    SDLK_ESCAPE,
    SDLK_F1,
    SDLK_F2,
    SDLK_F3,
    SDLK_F4,
    SDLK_F5,
    SDLK_F6,
    SDLK_F7,
    SDLK_F8,
    SDLK_F9,
    SDLK_F10,
    SDLK_F11,
    SDLK_F12,
    SDLK_BACKQUOTE,      
    SDLK_0,
    SDLK_1, 
    SDLK_2, 
    SDLK_3, 
    SDLK_4, 
    SDLK_5, 
    SDLK_6, 
    SDLK_7, 
    SDLK_8, 
    SDLK_9, 
    SDLK_MINUS,
    SDLK_EQUALS,
    SDLK_BACKSPACE,
    SDLK_TAB,
    SDLK_a, 
    SDLK_b, 
    SDLK_c, 
    SDLK_d, 
    SDLK_e, 
    SDLK_f, 
    SDLK_g, 
    SDLK_h, 
    SDLK_i, 
    SDLK_j, 
    SDLK_k, 
    SDLK_l, 
    SDLK_m, 
    SDLK_n, 
    SDLK_o, 
    SDLK_p, 
    SDLK_q, 
    SDLK_r, 
    SDLK_s, 
    SDLK_t, 
    SDLK_u, 
    SDLK_v, 
    SDLK_w, 
    SDLK_x, 
    SDLK_y, 
    SDLK_z, 
    SDLK_LSHIFT,
    SDLK_RSHIFT,
    SDLK_LCTRL,
    SDLK_RCTRL,
    SDLK_LALT,
    SDLK_RALT,
    SDLK_SPACE,
    SDLK_LEFTBRACKET,
    SDLK_RIGHTBRACKET, 
    SDLK_SEMICOLON,
    SDLK_QUOTE,
    SDLK_COMMA,
    SDLK_PERIOD,
    SDLK_SLASH,
    SDLK_BACKSLASH,
    SDLK_RETURN,
    SDLK_RETURN,
    SDLK_INSERT,
    SDLK_DELETE,
    SDLK_HOME,
    SDLK_END,        
    SDLK_PAGEUP,     
    SDLK_PAGEDOWN, 
    SDLK_UP,
    SDLK_RIGHT,
    SDLK_DOWN,
    SDLK_LEFT,
    SDLK_KP_0,
    SDLK_KP_1,
    SDLK_KP_2,
    SDLK_KP_3,
    SDLK_KP_4,
    SDLK_KP_5,
    SDLK_KP_6,
    SDLK_KP_7,
    SDLK_KP_8,
    SDLK_KP_9,
    SDLK_CAPSLOCK,
    SDLK_NUMLOCKCLEAR,
    SDLK_SCROLLLOCK,
    SDLK_SCROLLLOCK,
}};

namespace Script {

bool IsKeyPressed(JSContext *ctx, unsigned argc, JS::Value *vp){

    JS::CallArgs args = CallArgsFromVp(argc, vp);
    if(!Turbo::CheckForSingleArg(ctx, args, Turbo::Number, __func__))
        return false;

	SDL_PumpEvents();
	const Uint8 *keystate = SDL_GetKeyboardState(nullptr);
    
    args.rval().set(BOOLEAN_TO_JSVAL(keystate[SDL_GetScancodeFromKey(args[0].toNumber())]));
    return true;
}

bool IsAnyKeyPressed(JSContext *ctx, unsigned argc, JS::Value *vp){
	SDL_PumpEvents();
	int numkeys = 343;
	const Uint8 * const keystate = SDL_GetKeyboardState(&numkeys);
	for(int i = 0; i<numkeys; i++){
		if(keystate[i]){
            CallArgsFromVp(argc, vp).rval().set(JSVAL_TRUE);
            return true;
        }
	}
    
    CallArgsFromVp(argc, vp).rval().set(JSVAL_FALSE);
    return true;
}

}
}
