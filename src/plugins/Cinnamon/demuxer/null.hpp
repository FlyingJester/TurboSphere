#pragma once
#include "../libapi.hpp"

namespace Cinnamon{

// A NOP demuxer. Can be used to test against NULL and 0-length demuxer calls (which should never happen),
//   and against zero'd out decoder calls.
class NullDemuxer : public Demuxer {

    struct packet_info Demux(uint8_t * restrict into, size_t len, const uint8_t * restrict from, size_t remaining) override;

};

}
