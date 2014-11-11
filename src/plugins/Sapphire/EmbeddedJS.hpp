#pragma once

#include <array>
#include "Sapphire.hpp"

#define NUM_EMBEDDED_SCRIPTS 1

namespace Sapphire {
namespace Embed {

extern const std::array<const char * const, NUM_EMBEDDED_SCRIPTS> EmbeddedJSSource;

void ExecuteEmbdeddedJS(v8::Isolate *);

}
}
