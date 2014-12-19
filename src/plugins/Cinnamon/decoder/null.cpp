#include "null.hpp"
#include <cassert>

namespace Cinnamon {

struct Decoder::packet_info NullDecoder::Decode(uint8_t *restrict into, size_t len, const uint8_t *restrict from, size_t remaining){
    assert(into);
    assert(len);
    assert(from);
    assert(remaining);

    return {0ull, sFloating, 0ull, false, false, false};
}

}
