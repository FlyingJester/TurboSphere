#include "null.hpp"
#include <cassert>

struct Cinnamon::Demuxer::packet_info Cinnamon::NullDemuxer::Demux(uint8_t * restrict into, size_t len, const uint8_t * restrict from, size_t remaining){

    assert(into);
    assert(from);
    assert(len);
    assert(remaining);

    return {0, sFloating, 0, 0};
};
