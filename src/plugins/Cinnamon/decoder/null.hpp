#pragma once
#include"../libapi.hpp"

namespace Cinnamon{

class NullDecoder : public Decoder {
public:

    struct packet_info Decode(uint8_t *restrict into, size_t len, const uint8_t *restrict from, size_t remaining) override;

};

}
