#pragma once
#include <vector>
#include "../libapi.hpp"

namespace Cinnamon{

class RawDecoder : public Decoder {

    std::vector<uint8_t> buffer;

public:

    struct packet_info Decode(uint8_t * restrict into, size_t len, const uint8_t * restrict from, size_t remaining) override;

};

}
