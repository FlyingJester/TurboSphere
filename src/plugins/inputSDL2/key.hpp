#pragma once
#include "input_main.hpp"
#include <array>

#define NUM_KEYS 95

namespace InputSDL2{
    
    extern std::array<const char * const, NUM_KEYS> key_names;
    extern std::array<unsigned, NUM_KEYS> key_numbers; // Translates directly to an SDL key constant

    namespace Script{
        bool IsKeyPressed(JSContext *ctx, unsigned argc, JS::Value *vp);
        bool IsAnyKeyPressed(JSContext *ctx, unsigned argc, JS::Value *vp);
    }
}
