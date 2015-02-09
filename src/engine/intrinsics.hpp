#pragma once

#include <jsapi.h>

namespace Turbo{
    // Initializes the Sphere functions that TurboSphere (the engine, no plugins) provides.
    // Theoretically, we could pass a 'Sphere' object for global to put all of our intrinsics into
    // our own namespace.
    bool initializeIntrinsicFunctions(JSContext *ctx, JS::HandleObject global);
}
