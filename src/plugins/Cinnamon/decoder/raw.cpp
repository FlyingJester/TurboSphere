#include "raw.hpp"
#include <cmath>
#include <cassert>

namespace Cinnamon {

struct Decoder::packet_info RawDecoder::Decode(uint8_t *restrict into, size_t len, const uint8_t *restrict from, size_t remaining){

    assert(into);
    assert(from);
    assert(len);
    assert(remaining);

    if(!buffer.empty()){

        buffer.insert(buffer.cend()--, from, from+remaining);

        const std::vector<uint8_t>::const_iterator first = buffer.cbegin();
        const std::vector<uint8_t>::const_iterator last = std::min(buffer.cbegin()+len, buffer.cend());

        size_t size = last-first;

        std::copy(first, last, into);
        buffer.erase(first, last);

        return {size, sFloating, 441000, 4, 0, 0};

    }
    else{
        size_t to_copy = std::min<size_t>(len, remaining);
        std::copy_n(from, to_copy, into);

        return {to_copy, sFloating, 441000, 4, 0, 0};
    }
};

}
