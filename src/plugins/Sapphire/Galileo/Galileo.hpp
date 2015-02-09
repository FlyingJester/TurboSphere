#pragma once

#include <color.h>
#include "Group.hpp"
#include "Shape.hpp"
#include "State.hpp"
#include "Vertex.hpp"

namespace Sapphire{
namespace Galileo{
    /////
    // Sets up default GL state. Is thread-local.
    void Init(void *ctx, uint64_t ID);
}
}
