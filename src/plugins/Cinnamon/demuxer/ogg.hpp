#pragma once
#ifdef USE_OGG

#include <memory>
#include "../libapi.hpp"


namespace Cinnamon{

class OggDemuxer : public Demuxer {

    struct Implementation;
    std::unique_ptr<Implementation> guts;

public:

    OggDemuxer();

    // Returned data begins with char[32].
    //
    // The first byte is standard Cinnamon:
    // char[0]&(1<<0) is the begin-packet flag.
    // char[0]&(1<<1) is the mid-packet flag.
    // char[0]&(1<<2) is the end-packet flag.
    // char[0]&(1<<3) is the begin-stream flag, Xiph style
    // char[0]&(1<<4) is the end-stream flag, Xiph style
    //
    // char[1]...char[8] is the granule position.
    // char[9]...char[16] is the packet number.
    //
    // The returned size INCLUDES the size of the header.
    struct packet_info Demux(uint8_t * restrict into, size_t len, const uint8_t * restrict from, size_t remaining) override;

};

}

#endif //USE_OGG
