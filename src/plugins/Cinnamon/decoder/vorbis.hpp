#pragma once
#ifdef USE_VORBIS

#include <memory>
#include "../libapi.hpp"


namespace Cinnamon{

class VorbisDecoder : public Decoder {

    struct Implementation;
    std::unique_ptr<Implementation> guts;

public:

    VorbisDecoder();

    int Decode(void * restrict into, size_t len, void * restrict from, size_t remaining) override;

}

#endif // USE_VORBIS
